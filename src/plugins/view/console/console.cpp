#include "console.h"
#include <QtCore/QDebug>
#include <QtWidgets/QWidget>

#include "../../../plugin_api/HippoMessages.h"
#include "../../../plugin_api/HippoPlugin.h"
#include <QtWidgets/QLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QTextCursor>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include "ui_console.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* ConsoleView::create(struct HippoServiceAPI* service_api, QAbstractItemModel* model) {
    //m_metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_MESSAGE_API_VERSION);

    //m_text_font = new QFont("DejaVu Sans Mono", 10);
    m_base = new QWidget;
    m_ui = new Ui_Console();
    m_ui->setupUi(m_base);
    //m_main_layout = new QVBoxLayout(m_base);

    return m_base;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleView::event(const unsigned char* data, int len) {
    /*
    const HippoMessage* message = GetHippoMessage(data);

    switch (message->message_type()) {
        case MessageType_select_song: {
            select_song(message->message_as_select_song());
            break;
        }

        default:
            break;
    }
    */
}

