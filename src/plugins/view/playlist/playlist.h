#pragma once

#include "../../../plugin_api/HippoQtView.h"

class QWidget;
struct HippoMessageAPI;
struct HippoServiceAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlaylistView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "playlist_view.json")
    Q_INTERFACES(HippoQtView)

public:
    QWidget* create(struct HippoServiceAPI* service_api);
    void event(const HippoMessageDecode* message);

private:
    struct HippoMessageAPI* m_message_api = nullptr;
};


