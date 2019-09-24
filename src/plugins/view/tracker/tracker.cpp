#include "tracker.h"
#include "tracker_display.h"
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
    m_display = new TrackerDisplay(nullptr);

    return m_display;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerView::event(const unsigned char* data, int len) {
    m_display->event(data, len);
}

