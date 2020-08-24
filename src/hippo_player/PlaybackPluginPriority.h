#pragma once

#include <QDialog>

class Ui_PlaybackPluginPriority;
class QListWidgetItem;
class QListWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PlaybackPluginPriority is used to re-order playback plugins in their priority order
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlaybackPluginPriority : public QDialog {
    Q_OBJECT

public:
    explicit PlaybackPluginPriority(QWidget* parent = nullptr);
    virtual ~PlaybackPluginPriority();

private:
    Ui_PlaybackPluginPriority* m_ui = nullptr;
    QListWidget* m_plugin_list = nullptr;
};
