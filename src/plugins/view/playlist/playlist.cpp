#include "playlist.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>
#include "../../../plugin_api/HippoPlugin.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* PlaylistView::create(struct HippoServiceAPI* service_api) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    QWidget* widget = new QWidget;
    QVBoxLayout* vbox = new QVBoxLayout(widget);

    m_list = new QListWidget;

    vbox->QLayout::addWidget(m_list);

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::event(const unsigned char* data, int len) {
    printf("got event!\n");
    (void)data;
    (void)len;
}


