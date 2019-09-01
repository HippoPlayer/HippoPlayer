#pragma once

#include <QtCore/QObject>

struct HippoServiceAPI;

class HippoQtView {
public:
    virtual ~HippoQtView() {}

    // Create a the view for the plugin.
    virtual QWidget* create(struct HippoServiceAPI* service_api) = 0;
};

#define HippoQtView_iid "org.hippoplayer.HippoQtView"

Q_DECLARE_INTERFACE(HippoQtView, HippoQtView_iid)

