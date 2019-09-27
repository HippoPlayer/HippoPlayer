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

void PlaylistView::select_song(const HippoSelectSong* msg) {
	int playlist_index = msg->playlist_index();

	if (!msg->path()) {
		printf("Playlist: no correct path has been set for select_song message, unable to update playlist selection\n");
		return;
	}

	const char* path_name = msg->path()->c_str();
	int path_len = msg->path()->Length();

	// first just try to select the message based on the index, usually the slow path should never happen

	QListWidgetItem* item = m_list->item(playlist_index); 

	if (item) {
		QVariant v = item->data(Qt::UserRole);
		QByteArray path = v.toByteArray();

		if (!strncmp(path.data(), path_name, path_len)) {
			m_list->setCurrentItem(item);
			return;
		} else {
			printf("Playlist: Unable to set matching item: Expected %s but was %s for entry %d\n",
					path_name, path.data(), playlist_index);
		}
	} else {
		printf("Unable to set matching item for %s\n", path_name);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::item_double_clicked(QListWidgetItem* item) {
    flatbuffers::FlatBufferBuilder builder(1024);
    QVariant v = item->data(Qt::UserRole);
    QByteArray path = v.toByteArray();

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_select_song,
        CreateHippoRequestSelectSongDirect(builder, path.data(), m_list->row(item)).Union()));

    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::event(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    switch (message->message_type())
	{
		case MessageType_select_song:
		{
			select_song(message->message_as_select_song());
			break;
		}

    	case MessageType_reply_added_urls:
		{
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

			break;
		}
	}
}


