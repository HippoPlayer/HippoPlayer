#pragma once

#include "../../../plugin_api/HippoQtView.h"

class QWidget;
class ScrollText;
struct HippoMessageAPI;
struct HippoServiceAPI;
class QPushButton;
class QIcon;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlayerView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "player_view.json")
    Q_INTERFACES(HippoQtView)

public:
    QWidget* create(struct HippoServiceAPI* service_api, QAbstractItemModel* model);
    void incoming_messages(const unsigned char* data, int len);

    Q_SLOT void next_song();
    Q_SLOT void stop_song();
    Q_SLOT void play_song();
    Q_SLOT void prev_song();

    Q_SLOT void loop_current(int state);
    Q_SLOT void randomize_playlist(int state);

private:
    void set_pause_state(bool state);
    const struct HippoMessageAPI* m_message_api = nullptr;
    ScrollText* m_song_title = nullptr;
    QPushButton* m_play_pause_button = nullptr;
    QIcon* m_play_icon;
    QIcon* m_pause_icon;
    bool m_play_state = true;
    bool m_has_stopped = false;
};

