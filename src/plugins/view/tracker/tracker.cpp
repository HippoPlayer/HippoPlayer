#include "tracker.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "../../../plugin_api/HippoMessages.h"
#include "../../../plugin_api/HippoPlugin.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* TrackerView::create(struct HippoServiceAPI* service_api) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    QWidget* widget = new QWidget;

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerView::event(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    // Only care about added files right now
    if (message->message_type() != MessageType_select_song)
        return;

    auto title = message->message_as_select_song()->title();
}

