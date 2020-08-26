#include "player.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
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
    m_song_title->setText(QStringLiteral(""));

    QPushButton* prev_button = create_button("bin/player/buttons/hip_button_back.png");
    QPushButton* stop_button = create_button("bin/player/buttons/hip_button_stop.png");
    QPushButton* play_button = create_button("bin/player/buttons/hip_button_play.png");
    QPushButton* next_button = create_button("bin/player/buttons/hip_button_next.png");

    QObject::connect(prev_button, &QPushButton::pressed, this, &PlayerView::prev_song);
    QObject::connect(next_button, &QPushButton::pressed, this, &PlayerView::next_song);
    QObject::connect(stop_button, &QPushButton::pressed, this, &PlayerView::stop_song);
    QObject::connect(play_button, &QPushButton::pressed, this, &PlayerView::play_song);

    m_play_icon = new QIcon("bin/player/buttons/hip_button_play.png");
    m_pause_icon = new QIcon("bin/player/buttons/hip_button_pause.png");

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(prev_button);
    hbox->addWidget(stop_button);
    hbox->addWidget(play_button);
    hbox->addWidget(next_button);

    m_play_pause_button = play_button;

    QCheckBox* loop_current = new QCheckBox(QStringLiteral("Loop Current"));
    QCheckBox* randomize_playlist = new QCheckBox(QStringLiteral("Randomize Playlist"));
    QHBoxLayout* extra_buttons_layout = new QHBoxLayout;

    QObject::connect(loop_current, &QCheckBox::stateChanged, this, &PlayerView::loop_current);
    QObject::connect(randomize_playlist, &QCheckBox::stateChanged, this, &PlayerView::randomize_playlist);

    extra_buttons_layout->addWidget(loop_current);
    extra_buttons_layout->addWidget(randomize_playlist);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(m_song_title);
    vbox->addLayout(hbox);
    vbox->addLayout(extra_buttons_layout);

    widget->setLayout(vbox);

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::incoming_messages(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    if (message->message_type() != MessageType_select_song)
        return;

    auto title = message->message_as_select_song()->description()->title();

    m_song_title->setText(QString::fromUtf8(title->c_str(), title->size()));
    m_play_pause_button->setIcon(*m_pause_icon);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::prev_song() {
    set_pause_state(false);

    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_prev_song, CreateHippoPrevSong(builder).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::next_song() {
    set_pause_state(false);

    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_next_song, CreateHippoNextSong(builder).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::stop_song() {
    set_pause_state(false);

    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_stop_song, CreateHippoStopSong(builder).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());

    m_has_stopped = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::set_pause_state(bool state) {
    if (state) {
        m_play_pause_button->setIcon(*m_pause_icon);
    } else {
        m_play_pause_button->setIcon(*m_play_icon);
    }

    m_play_state = state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::play_song() {
    set_pause_state(m_play_state);
    m_play_state = !m_play_state;

    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_play_song,
    CreateHippoRequestPlaySong(builder, m_play_state, m_has_stopped).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());

    m_has_stopped = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::loop_current(int state) {
    flatbuffers::FlatBufferBuilder builder(1024);

    builder.Finish(CreateHippoMessageDirect(
        builder, MessageType_loop_current,
        CreateHippoLoopCurrent(builder, state == Qt::Checked).Union()));

    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::randomize_playlist(int state) {
    flatbuffers::FlatBufferBuilder builder(1024);

    builder.Finish(CreateHippoMessageDirect(
        builder, MessageType_randomize_playlist,
        CreateHippoLoopCurrent(builder, state == Qt::Checked).Union()));

    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

