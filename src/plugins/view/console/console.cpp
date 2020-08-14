#include "console.h"
#include <QtCore/QDebug>
#include <QtWidgets/QWidget>

#include <QtGui/QTextCursor>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QSyntaxHighlighter>
#include "../../../plugin_api/HippoMessages.h"
#include "../../../plugin_api/HippoPlugin.h"
#include "ui_console.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Highlighter : public QSyntaxHighlighter {

public:
    Highlighter(QTextDocument* parent = 0);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlighting_rules;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static const char* level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};

static QColor s_colors[] = {
    QColor(92, 92, 255),    // Trace (light blue)
    QColor(0, 205, 205),    // Debug (Cyan)
    QColor(0, 205, 0),      // Info (Green)
    QColor(205, 205, 0),    // Warn (Yellow),
    QColor(205, 0, 0),      // Error (Red),
    QColor(205, 0, 205),    // Debug (magneta)
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static QString s_warn_levels[] = {
    QStringLiteral("TRACE"),
    QStringLiteral("DEBUG"),
    QStringLiteral("INFO"),
    QStringLiteral("WARN"),
    QStringLiteral("ERROR"),
    QStringLiteral("FATAL"),
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Highlighter::Highlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
    HighlightingRule category;
    category.format.setForeground(Qt::darkGray);
    category.pattern = QRegularExpression(QStringLiteral("\\[.*\\]"));
    highlighting_rules.append(category);

    for (int i = 0; i < 6; ++i) {
        HighlightingRule rule;
        rule.format.setForeground(s_colors[i]);
        rule.pattern = QRegularExpression(s_warn_levels[i]);
        highlighting_rules.append(rule);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Highlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule& rule : qAsConst(highlighting_rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);

        //qDebug() << "matching " << text << " with " << rule.pattern;

        while (matchIterator.hasNext()) {
            //qDebug() << "matched";
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* ConsoleView::create(struct HippoServiceAPI* service_api, QAbstractItemModel* model) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    // m_metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_MESSAGE_API_VERSION);

    m_base = new QWidget;
    m_ui = new Ui_Console();
    m_ui->setupUi(m_base);
    m_ui->log_messages->setFont(QFont("DejaVu Sans Mono", 10));
    m_ui->log_messages->setLineWrapMode(QPlainTextEdit::NoWrap);

    m_highlighter = new Highlighter(m_ui->log_messages->document());

    // m_main_layout = new QVBoxLayout(m_base);

    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_log_send_messages,
                                            CreateHippoLogSendMessages(builder, true).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());

    return m_base;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConsoleView::~ConsoleView() {
    // disable log sending
    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_log_send_messages,
                                            CreateHippoLogSendMessages(builder, false).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());

    delete m_highlighter;

    m_highlighter = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::log_messages(const struct HippoLogMessages* messages) {
    for (auto const& msg : *messages->messages()) {
        QString text = QString::fromUtf8(msg->c_str());
        m_ui->log_messages->appendPlainText(text);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::event(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    switch (message->message_type()) {
        case MessageType_log_messages: {
            log_messages(message->message_as_log_messages());
            break;
        }

        default:
            break;
    }
}
