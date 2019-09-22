#pragma once

#include "../../../plugin_api/HippoQtView.h"

class QWidget;
class ScrollText;
struct HippoMessageAPI;
struct HippoServiceAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TrackerView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "tracker_view.json")
    Q_INTERFACES(HippoQtView)

public:
    QWidget* create(struct HippoServiceAPI* service_api);
    void event(const unsigned char* data, int len);

private:
    const struct HippoMessageAPI* m_message_api = nullptr;
};


