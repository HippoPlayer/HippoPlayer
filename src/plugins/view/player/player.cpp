#include "player.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "../../../plugin_api/HippoPlugin.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static QPushButton* create_button(const QString& icon_filename) {
    const QIcon icon(icon_filename);
    return new QPushButton(icon, QStringLiteral(""));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* PlayerView::create(struct HippoServiceAPI* service_api) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    QWidget* widget = new QWidget;

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

    widget->setLayout(hbox);

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::prev_song() {
    HippoMessage_playlist_prev_song(m_message_api);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::next_song() {
    HippoMessage_playlist_next_song(m_message_api);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::stop_song() {
    HippoMessage_stop_song(m_message_api);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerView::play_song() {
    HippoMessage_play_song(m_message_api);
}