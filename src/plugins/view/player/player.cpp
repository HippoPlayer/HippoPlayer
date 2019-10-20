#include "player.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "../../../plugin_api/HippoMessages.h"
#include "../../../plugin_api/HippoPlugin.h"
#include "scrolltext.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static QPushButton* create_button(const QString& icon_filename) {
    const QIcon icon(icon_filename);
    return new QPushButton(icon, QStringLiteral(""));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* PlayerView::create(struct HippoServiceAPI* service_api, QAbstractItemModel* model) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    QWidget* widget = new QWidget;

    m_song_title = new ScrollText(widget);
    m_song_title->setText(QStringLiteral("This is a very long tile that shouldn't be that long"));

    QPushButton* prev_button = create_button("bin/player/buttons/hip_button_back.png");
    QPushButton* stop_button = create_button("bin/player/buttons/hip_button_stop.png");
    QPushButton* play_button = create_button("bin/player/buttons/hip_button_play.png");
    QPushButton* next_button = create_button("bin/player/buttons/hip_button_next.png");

    QObject::connect(prev_button, &QPushButton::pressed, this, &PlayerView::prev_song);
    QObject::connect(next_button, &QPushButton::pressed, this, &PlayerView::next_song);
    QObject::connect(stop_button, &QPushButton::pressed, this, &PlayerView::stop_song);
    QObject::connect(play_button, &QPushButton::pressed, this, &PlayerView::play_song);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(prev_button);
    hbox->addWidget(stop_button);
    hbox->addWidget(play_button);
    hbox->addWidget(next_button);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(m_song_title);
    vbox->addLayout(hbox);

    widget->setLayout(vbox);

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::event(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    if (message->message_type() != MessageType_select_song)
        return;

    auto title = message->message_as_select_song()->description()->title();

    m_song_title->setText(QString::fromUtf8(title->c_str(), title->size()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::prev_song() {
    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_prev_song, CreateHippoPrevSong(builder).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::next_song() {
	printf("next song\n");
    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_next_song, CreateHippoNextSong(builder).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::stop_song() {
    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_stop_song, CreateHippoStopSong(builder).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::play_song() {
    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_play_song, CreateHippoPlaySong(builder).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}
