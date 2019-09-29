#pragma once

#include "../../../plugin_api/HippoQtView.h"

class QWidget;
class ScrollText;
struct HippoMessageAPI;
struct HippoServiceAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlayerView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "player_view.json")
    Q_INTERFACES(HippoQtView)

public:
    QWidget* create(struct HippoServiceAPI* service_api, QAbstractItemModel* model);
    void event(const unsigned char* data, int len);

    Q_SLOT void next_song();
    Q_SLOT void stop_song();
    Q_SLOT void play_song();
    Q_SLOT void prev_song();

private:
    const struct HippoMessageAPI* m_message_api = nullptr;
    ScrollText* m_song_title = nullptr;
};

