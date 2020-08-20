#include "DevicePanel.h"
#include "ui_DevicePanel.h"
#include "../../../plugin_api/HippoMessages.h"
#include "../../../plugin_api/HippoPlugin.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DevicePanel::DevicePanel(const struct HippoMessageAPI* messages_api, QWidget* parent)
    : QDialog(parent), m_ui(new Ui_DevicePanel), m_messages_api(messages_api) {
    m_ui->setupUi(this);

    // Setup for default device with values that seems reasonable
    auto t = DeviceInfo { 1, 2, 11025, 192000 };
    m_device_info.push_back(t);

    QObject::connect(
        m_ui->device_name, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &DevicePanel::change_device);

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
    m_device_info.clear();

    // Setup for default device with values that seems reasonable
    auto t = DeviceInfo { 1, 2, 11025, 192000 };
    m_device_info.push_back(t);

    for (auto const& msg : *messages->devices()) {
        const char* device_name = msg->name()->c_str();
        m_ui->device_name->addItem(QString::fromUtf8(device_name));
        auto t = DeviceInfo {
            msg->min_channels(),
            msg->max_channels(),
            msg->min_sample_rate(),
            msg->max_sample_rate(),
        };

        m_device_info.push_back(t);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard sample rates we setup

static uint32_t s_standard_sample_rates[] = {
    8000,
    11025,
    16000,
    22050,
    24000,
    32000,
    44100,
    48000,
    88200,
    96000,
    176400,
    192000,
    352800,
    384000,
};

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DevicePanel::change_device(int index) {
    m_ui->sample_rate->clear();
    m_ui->channels->clear();

    const auto& device = m_device_info[index];

    int max_channel_count = 32;

    for (int i = device.min_channels; i <= device.max_channels; ++i) {
        if (--max_channel_count == 0)
            break;

        m_ui->channels->addItem(QString::number(i));
    }

    // find range for our sample rates

    int min_start_index = 0;
    int max_start_index = 0;

    printf("min range %d max range %d\n", device.min_sample_rate, device.max_sample_rate);

    for (size_t i = 0; i < sizeof_array(s_standard_sample_rates); ++i) {
        int current_rate = s_standard_sample_rates[i];

        if (current_rate <= device.min_sample_rate) {
            min_start_index = i;
        }

        if (current_rate >= device.max_sample_rate) {
            max_start_index = i;
        }
    }

    // if we get no sample rate back just use defaults
    if (device.min_sample_rate == 0 || device.max_sample_rate == 0) {
        min_start_index = 0;
        max_start_index = sizeof_array(s_standard_sample_rates) - 1;
    }

    printf("range %d %d\n", min_start_index, max_start_index);

    for (int i = min_start_index; i <= max_start_index; ++i) {
        m_ui->sample_rate->addItem(QString::number(s_standard_sample_rates[i]));
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
