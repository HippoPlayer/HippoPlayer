#include "DevicePanel.h"
#include "ui_DevicePanel.h"
#include "../../../plugin_api/HippoMessages.h"
#include "../../../plugin_api/HippoPlugin.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DevicePanel::DevicePanel(const struct HippoMessageAPI* messages_api, QWidget* parent)
    : QDialog(parent), m_ui(new Ui_DevicePanel), m_messages_api(messages_api) {
    m_ui->setupUi(this);

    flatbuffers::FlatBufferBuilder builder(1024);

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_output_devices,
                                            CreateHippoRequestOutputDevices(builder).Union()));
    HippoMessageAPI_send(m_messages_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DevicePanel::~DevicePanel() {
    delete m_ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DevicePanel::get_devices(const struct HippoReplyOutputDevices* messages) {
    m_ui->device_name->clear();
    m_ui->device_name->addItem(QStringLiteral("Default Sound Device"));

    for (auto const& msg : *messages->devices()) {
        const char* device_name = msg->name()->c_str();
        m_ui->device_name->addItem(QString::fromUtf8(device_name));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DevicePanel::incoming_messages(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    switch (message->message_type()) {
        case MessageType_reply_output_devices: {
            get_devices(message->message_as_reply_output_devices());
            break;
        }

        default:
            break;
    }
}

/*
    flatbuffers::FlatBufferBuilder builder(1024);

    auto filename = builder.CreateString(log_file.toUtf8().constData());

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_log_file,
                                            CreateHippoLogToFile(builder, filename, log_to_file).Union()));
    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
*/
