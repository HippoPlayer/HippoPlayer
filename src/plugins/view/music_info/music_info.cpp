#include "music_info.h"
#include <QtCore/QDebug>
#include <QtWidgets/QWidget>

#include "../../../plugin_api/HippoMessages.h"
#include "../../../plugin_api/HippoPlugin.h"
#include <QtWidgets/QLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QTextCursor>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* MusicInfoView::create(struct HippoServiceAPI* service_api, QAbstractItemModel* model) {
    m_metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_MESSAGE_API_VERSION);

    m_text_font = new QFont("Courier", 12);
    m_base = new QWidget;
    m_main_layout = new QVBoxLayout(m_base);

    return m_base;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MusicInfoView::create_label_text(const QString& label_name, const QString& text) {
    auto layout = new QHBoxLayout;

    auto label_text = new QLabel(label_name);
    auto line = new QLineEdit(text);

    line->setReadOnly(true);

    layout->addWidget(label_text);
    layout->addWidget(line);

    m_widgets.push_back(label_text);
    m_widgets.push_back(line);

    m_main_layout->addLayout(layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QPlainTextEdit* MusicInfoView::create_label_multi(const QString& label_name) {
    auto layout = new QHBoxLayout;

    auto label_text = new QLabel(label_name);
    auto line = new QPlainTextEdit;

    line->setFont(*m_text_font);

    layout->addWidget(label_text);
    layout->addWidget(line);

    m_widgets.push_back(label_text);
    m_widgets.push_back(line);

    m_main_layout->addLayout(layout);

    return line;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MusicInfoView::select_song(const HippoSelectSong* msg) {
    QLayoutItem* child = nullptr;

    while ((child = m_main_layout->takeAt(0)) != 0) {
        delete child;
    }

    for (auto& t : m_widgets) {
        delete t;
    }

    m_widgets.clear();

    const char* url = msg->description()->path()->c_str();

    int count = HippoMetadata_begin_get_all(m_metadata_api, url);

    for (int i = 0; i < count; ++i) {
        const char* name = nullptr;
        const char* data = nullptr;
        int name_len = 0;
        int data_len = 0;

        HippoMetadata_get_all_entry(m_metadata_api, i, &name, &data, &name_len, &data_len);

        QString n = QString::fromUtf8(name, name_len);
        QString d = QString::fromUtf8(data, data_len);

        if (d == "") {
            continue;
        }

        // if text contains multiline we create a multiline widget instead

        if (d.contains(QChar::LineFeed)) {
            auto widget = create_label_multi(n);
            widget->insertPlainText(d);
        } else {
            create_label_text(n, d);
        }

        //qDebug() << n << d;
    }

    const char* text = nullptr;
    int text_len;
    int index = 0;

    QPlainTextEdit* samples = nullptr;
    QPlainTextEdit* instruments = nullptr;

    while (HippoMetadata_get_all_sample(m_metadata_api, index, &text, &text_len)) {
        QString t = QString::fromUtf8(text, text_len);
        if (!samples) {
            samples = create_label_multi(QStringLiteral("Samples"));
        }

        samples->insertPlainText(t);
        samples->insertPlainText(QStringLiteral("\n"));
        index++;
    }

    index = 0;

    while (HippoMetadata_get_all_instrument(m_metadata_api, index, &text, &text_len)) {
        QString t = QString::fromUtf8(text, text_len);
        if (!instruments) {
            instruments = create_label_multi(QStringLiteral("Instruments"));
        }

        instruments->insertPlainText(t);
        instruments->insertPlainText(QStringLiteral("\n"));
        index++;
    }

    if (instruments) {
        QTextCursor cursor = instruments->textCursor();
        cursor.movePosition(QTextCursor::Start);
        instruments->setTextCursor(cursor);
        instruments->setReadOnly(true);
    }

    if (samples) {
        QTextCursor cursor = samples->textCursor();
        cursor.movePosition(QTextCursor::Start);
        samples->setTextCursor(cursor);
        samples->setReadOnly(true);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MusicInfoView::event(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    switch (message->message_type()) {
        case MessageType_select_song: {
            select_song(message->message_as_select_song());
            break;
        }

        default:
            break;
    }
}
