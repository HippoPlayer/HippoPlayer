#include "playlist.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QListView>
#include "../../../plugin_api/HippoPlugin.h"
#include "../../../plugin_api/HippoMessages.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* PlaylistView::create(struct HippoServiceAPI* service_api, QAbstractItemModel* model) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    QWidget* widget = new QWidget;
    QVBoxLayout* vbox = new QVBoxLayout(widget);

    m_list = new QListView;
    m_list->setModel(model);

    m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    vbox->QLayout::addWidget(m_list);

    QObject::connect(m_list, &QListView::doubleClicked, this, &PlaylistView::item_double_clicked);

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

	const QModelIndex item = m_list->model()->index(playlist_index, 0);
	m_list->setCurrentIndex(item);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::item_double_clicked(const QModelIndex& item) {
    QAbstractItemModel* model = m_list->model();

    flatbuffers::FlatBufferBuilder builder(1024);
    QVariant v = model->data(item, Qt::UserRole);
    QByteArray path = v.toByteArray();

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_select_song,
        CreateHippoRequestSelectSongDirect(builder, path.data(), model->buddy(item).row()).Union()));

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

		default: break;
	}
}


