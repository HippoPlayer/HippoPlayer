#include "playlist.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include "../../../plugin_api/HippoPlugin.h"
#include "../../../plugin_api/HippoMessages.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* PlaylistView::create(struct HippoServiceAPI* service_api) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    QWidget* widget = new QWidget;
    QVBoxLayout* vbox = new QVBoxLayout(widget);

    m_list = new QListWidget;

    vbox->QLayout::addWidget(m_list);

    QObject::connect(m_list, &QListWidget::itemDoubleClicked, this, &PlaylistView::item_double_clicked);

    // Have the core send us the current list of songs
    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_added_urls,
        CreateHippoRequestAddedUrls(builder).Union()));

    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::item_double_clicked(QListWidgetItem* item) {
    flatbuffers::FlatBufferBuilder builder(1024);
    QVariant v = item->data(Qt::UserRole);
    QByteArray path = v.toByteArray();

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_select_song,
        CreateHippoRequestSelectSongDirect(builder, path.data()).Union()));

    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::event(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    // Only care about added files right now
    if (message->message_type() != MessageType_reply_added_urls)
        return;

    auto urls = message->message_as_reply_added_urls()->urls();

    for (int i = 0, e = urls->Length(); i < e; ++i) {
        auto url = urls->Get(i);

        auto path = url->path();
        auto title = url->title();

        QListWidgetItem* item = new QListWidgetItem(QString::fromUtf8(title->c_str(), title->size()));

        // Store data as byte array as we are going to use this for the player backend that uses UTF-8
        item->setData(Qt::UserRole, QByteArray(path->c_str(), path->size()));

        m_list->addItem(item);
    }
}


