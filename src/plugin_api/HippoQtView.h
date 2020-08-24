#pragma once

#include <QtCore/QObject>

struct HippoServiceAPI;
class QAbstractItemModel;

class HippoQtView {
public:
    virtual ~HippoQtView() {}

    // Create a the view for the plugin.
    virtual QWidget* create(struct HippoServiceAPI* service_api, QAbstractItemModel* model = nullptr) = 0;

    // Events get sent with messages such as files added to playlist, new song etc
    virtual void incoming_messages(const unsigned char* data, int len) = 0;
};


#define HippoQtView_iid "org.hippoplayer.HippoQtView"

Q_DECLARE_INTERFACE(HippoQtView, HippoQtView_iid)

