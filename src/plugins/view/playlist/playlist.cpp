#include "playlist.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QWidget>
#include "../../../plugin_api/HippoPlugin.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* PlaylistView::create(struct HippoServiceAPI* service_api) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::event(const HippoMessageDecode* message) {
    (void)message;
}


