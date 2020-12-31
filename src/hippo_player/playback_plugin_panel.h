#pragma once

#include <QDialog>

class Ui_PlaybackPluginPanel;
class QListWidgetItem;
class QListWidget;
struct HippoCore;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PlaybackPluginPanel is used to re-order playback plugins in their priority order
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlaybackPluginPanel : public QDialog {
    Q_OBJECT

public:
    explicit PlaybackPluginPanel(const struct HippoCore* core, QWidget* parent = nullptr);
    virtual ~PlaybackPluginPanel();

private:
    const struct HippoCore* m_core;
    Ui_PlaybackPluginPanel* m_ui = nullptr;
    QListWidget* m_plugin_list = nullptr;
};

