#include "playlist.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>
#include "../../../plugin_api/HippoPlugin.h"
#include "../../../plugin_api/HippoMessages.h"

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
    const HippoMessage* message = GetHippoMessage(data);

    // Only care about added files right now
    if (message->message_type() != MessageType_reply_added_urls)
        return;

    auto t = message->message_as_reply_added_urls();
    printf("%p\n", t);

    auto urls = t->urls();

    printf("%p\n", urls);

    for (int i = 0, e = urls->Length(); i < e; ++i) {
        auto url = urls->Get(i);

        auto path = url->path();
        //auto title = url->title();
        //float duration = url->length();

        m_list->addItem(QString::fromUtf8(path->c_str()));
    }
}


