#pragma once

#include "../../../plugin_api/HippoQtView.h"

class QWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlayerView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "player_view.json")
    Q_INTERFACES(HippoQtView)

public:
    virtual QWidget* create();
};

