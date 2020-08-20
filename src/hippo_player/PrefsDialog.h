#pragma once

#include <QDialog>

class DevicePanel;
class PlaybackPluginPriority;
class QTreeWidgetItem;
class Ui_PrefsDialog;
struct HippoMessageAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PrefsDialog is the top view when opening up the preferences settings in the UI.
// It handles the various settings that can be selected on the left side and then it's up to
// each implementation of widget to handle the other settings.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PrefsDialog : public QDialog {
    Q_OBJECT

public:
    explicit PrefsDialog(const struct HippoMessageAPI* messages_api, QWidget* parent = nullptr);
    virtual ~PrefsDialog();

    void incoming_messages(const unsigned char* data, int len);

private:
    Q_SLOT void dialog_finished(int state);

    PlaybackPluginPriority* m_plugin_priority;
    DevicePanel* m_device_panel;

    Q_SLOT void change_layout(QTreeWidgetItem* curr, QTreeWidgetItem* prev);
    Ui_PrefsDialog* m_ui = nullptr;
    const struct HippoMessageAPI* m_messages_api = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

