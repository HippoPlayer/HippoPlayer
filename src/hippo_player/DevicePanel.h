#pragma once

#include <QDialog>
#include <vector>

class Ui_DevicePanel;
class QListWidgetItem;
class QListWidget;
struct HippoMessageAPI;
struct HippoReplyOutputDevices;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DevicePanel is used to select the output device for playback
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DevicePanel : public QDialog {
    Q_OBJECT

public:
    explicit DevicePanel(const struct HippoMessageAPI* messages_api, QWidget* parent = nullptr);
    virtual ~DevicePanel();

    void incoming_messages(const unsigned char* data, int len);

private:
    void get_devices(const struct HippoReplyOutputDevices* messages);

    struct DeviceInfo {
        int min_channels;
        int max_channels;
        int min_sample_rate;
        int max_sample_rate;
    };

    Q_SLOT void change_device(int index);

    std::vector<DeviceInfo> m_device_info;

    Ui_DevicePanel* m_ui = nullptr;
    const struct HippoMessageAPI* m_messages_api = nullptr;
};
