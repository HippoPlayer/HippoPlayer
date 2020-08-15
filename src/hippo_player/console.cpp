#include "console.h"
#include <QtCore/QDebug>
#include <QtWidgets/QWidget>

#include <QtCore/QRegularExpression>
#include <QtCore/QSettings>
#include <QtGui/QTextCursor>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
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

enum LogLevelMasks {
    Level_Trace = 1 << 1,
    Level_Debug = 1 << 2,
    Level_Info  = 1 << 3,
    Level_Warn  = 1 << 4,
    Level_Error = 1 << 5,
    Level_Fatal = 1 << 6,
    Level_All = Level_Trace | Level_Debug | Level_Info | Level_Warn | Level_Error | Level_Fatal,
};

const int LOG_LEVELS_COUNT = 6;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Highlighter::Highlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
    HighlightingRule category;
    category.format.setForeground(Qt::darkGray);
    category.pattern = QRegularExpression(QStringLiteral("\\[.*\\]"));
    highlighting_rules.append(category);

    for (int i = 0; i < LOG_LEVELS_COUNT; ++i) {
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

        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConsoleView::ConsoleView(const struct HippoMessageAPI* messages_api, QWidget* parent) : QWidget(parent) {
    m_message_api = messages_api;

    // m_metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_MESSAGE_API_VERSION);

    m_active_mask = Level_All;
    m_ui = new Ui_Console();
    m_ui->setupUi(this);
    m_ui->log_messages->setFont(QFont(QStringLiteral("DejaVu Sans Mono"), 10));
    m_ui->log_messages->setLineWrapMode(QPlainTextEdit::NoWrap);

    m_highlighter = new Highlighter(m_ui->log_messages->document());

    m_ui->trace_check->setChecked(true);
    m_ui->debug_check->setChecked(true);
    m_ui->info_check->setChecked(true);
    m_ui->warn_check->setChecked(true);
    m_ui->error_check->setChecked(true);
    m_ui->fatal_check->setChecked(true);

    QObject::connect(m_ui->trace_check, &QCheckBox::toggled, this, &ConsoleView::checked);
    QObject::connect(m_ui->debug_check, &QCheckBox::toggled, this, &ConsoleView::checked);
    QObject::connect(m_ui->info_check, &QCheckBox::toggled, this, &ConsoleView::checked);
    QObject::connect(m_ui->warn_check, &QCheckBox::toggled, this, &ConsoleView::checked);
    QObject::connect(m_ui->error_check, &QCheckBox::toggled, this, &ConsoleView::checked);
    QObject::connect(m_ui->fatal_check, &QCheckBox::toggled, this, &ConsoleView::checked);

    QObject::connect(m_ui->log_file, &QCheckBox::toggled, this, &ConsoleView::select_log_file);
    QObject::connect(m_ui->clear_button, &QCheckBox::pressed, this, &ConsoleView::clear_log);

    read_settings();

    send_log_enable(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConsoleView::~ConsoleView() {
    printf("writing settings\n");
    write_settings();
    // disable log sending
    send_log_enable(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::send_log_enable(bool state) {
    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_log_send_messages,
                                            CreateHippoLogSendMessages(builder, state).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::log_messages(const struct HippoLogMessages* messages) {
    for (auto const& msg : *messages->messages()) {
        const char* text = msg->c_str();
        QString qt_text = QString::fromUtf8(msg->c_str());

        // this is a fast matcher by looking for the end ] marke. Now this would break if some plugin is named ] but
        // that is in practice not something that will be done so we will live with that
        // we really don't need to do strlen here but here for safe
        size_t len = strlen(text);

        // assume visible if we can't find the level
        uint8_t mask = 0xff;

        for (size_t i = 0; i < len; ++i) {
            if (text[i] == ']') {
                switch (text[i + 2]) {
                    case 'T' : mask = Level_Trace; break;
                    case 'D' : mask = Level_Debug; break;
                    case 'I' : mask = Level_Info; break;
                    case 'W' : mask = Level_Warn; break;
                    case 'E' : mask = Level_Error; break;
                    case 'F' : mask = Level_Fatal; break;
                }
            }
        }

        m_line_masks.push_back(mask);
        m_ui->log_messages->appendPlainText(qt_text);

        if (m_active_mask != Level_All) {
            if (!(mask & m_active_mask)) {
                QTextBlock block = m_ui->log_messages->document()->lastBlock();
                block.setVisible(false);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::checked(bool) {
    uint16_t new_mask = 0;

    new_mask |= uint8_t(m_ui->trace_check->isChecked()) << 1;
    new_mask |= uint8_t(m_ui->debug_check->isChecked()) << 2;
    new_mask |= uint8_t(m_ui->info_check->isChecked()) << 3;
    new_mask |= uint8_t(m_ui->warn_check->isChecked()) << 4;
    new_mask |= uint8_t(m_ui->error_check->isChecked()) << 5;
    new_mask |= uint8_t(m_ui->fatal_check->isChecked()) << 6;

    // if no update was done we don't need to do anything
    if (new_mask == m_active_mask)
        return;

    auto doc = m_ui->log_messages->document();
    size_t block_count = doc->blockCount();

    if (block_count != m_line_masks.size()) {
        /*
        printf("ERROR: Unable to match lines for console %ld %ld\n", block_count, m_line_masks.size());

        for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next()) {
            qDebug() << it.text();
        }
        */

        return;
    }
    int i = 0;

    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next(), ++i) {
        uint8_t mask = m_line_masks[i];
        if (mask & new_mask) {
            it.setVisible(true);
        } else {
            it.setVisible(false);
        }
    }

    m_ui->log_messages->viewport()->update();

    m_active_mask = new_mask;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::select_log_file(bool checked) {
    bool log_to_file = false;
    QString log_file;

    if (checked) {
        log_file = QFileDialog::getSaveFileName(this, tr("Select Log File"), m_log_file, tr("Log files (*.txt)"),  nullptr,
                                QFileDialog::DontUseNativeDialog);

        if (log_file.isEmpty()) {
            m_ui->log_file->setChecked(false);
        } else {
            log_to_file = true;
            m_log_file = log_file;

            QFile file(m_log_file);
            bool writeable = file.open(QIODevice::WriteOnly);

            if (!writeable) {
                m_ui->log_file->setChecked(false);
                QMessageBox::critical(
                    this,
                    m_log_file,
                    tr("Unable to write to this location. Make sure you have permissions to write to this file."));
                return;
            }

            file.close();
        }
    }

    flatbuffers::FlatBufferBuilder builder(1024);

    auto filename = builder.CreateString(log_file.toUtf8().constData());

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_log_file,
                                            CreateHippoLogToFile(builder, filename, log_to_file ).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::clear_log() {
    m_ui->log_messages->clear();
    m_line_masks.clear();

    flatbuffers::FlatBufferBuilder builder(1024);

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_log_clear,
                                            CreateHippoLogClear(builder).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::write_settings() {
    QSettings settings;
    settings.beginGroup(QStringLiteral("console"));
    settings.setValue(QStringLiteral("log_file"), m_log_file);
    settings.setValue(QStringLiteral("trace"), m_ui->trace_check->isChecked());
    settings.setValue(QStringLiteral("debug"), m_ui->debug_check->isChecked());
    settings.setValue(QStringLiteral("info"), m_ui->info_check->isChecked());
    settings.setValue(QStringLiteral("warn"), m_ui->warn_check->isChecked());
    settings.setValue(QStringLiteral("error"), m_ui->error_check->isChecked());
    settings.setValue(QStringLiteral("fatal"), m_ui->fatal_check->isChecked());
    settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::read_settings() {
    QSettings settings;
    settings.beginGroup(QStringLiteral("console"));
    m_log_file = settings.value(QStringLiteral("log_file")).toString();
    m_ui->trace_check->setChecked(settings.value(QStringLiteral("trace"), QVariant(true)).toBool());
    m_ui->debug_check->setChecked(settings.value(QStringLiteral("debug"), QVariant(true)).toBool());
    m_ui->info_check->setChecked(settings.value(QStringLiteral("info"), QVariant(true)).toBool());
    m_ui->warn_check->setChecked(settings.value(QStringLiteral("warn"), QVariant(true)).toBool());
    m_ui->error_check->setChecked(settings.value(QStringLiteral("error"), QVariant(true)).toBool());
    m_ui->fatal_check->setChecked(settings.value(QStringLiteral("fatal"), QVariant(true)).toBool());
    m_ui->info_check->setChecked(settings.value(QStringLiteral("info"), QVariant(true)).toBool());
    settings.endGroup();

    m_active_mask = 0;
    m_active_mask |= uint8_t(m_ui->trace_check->isChecked()) << 1;
    m_active_mask |= uint8_t(m_ui->debug_check->isChecked()) << 2;
    m_active_mask |= uint8_t(m_ui->info_check->isChecked()) << 3;
    m_active_mask |= uint8_t(m_ui->warn_check->isChecked()) << 4;
    m_active_mask |= uint8_t(m_ui->error_check->isChecked()) << 5;
    m_active_mask |= uint8_t(m_ui->fatal_check->isChecked()) << 6;
}

