#include "MainWindow.h"
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QPluginLoader>
#include <QtCore/QStringListModel>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenuBar>
#include <QtGui/QFontDatabase>
#include <flatbuffers/flatbuffers.h>
#include "PlaylistModel.h"
#include "../../src/plugin_api/HippoPlugin.h"
#include "../../src/plugin_api/HippoMessages.h"
#include "../../src/plugin_api/HippoQtView.h"
//#include "src/external/qt_advanced_docking_system/src/DockManager.h"
//#include "src/external/qt_advanced_docking_system/src/DockWidget.h"
#include "src/external/toolwindowmanager/src/ToolWindowManager.h"
extern "C" {
	#include "src/hippo_core/native/hippo_core.h"
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(HippoCore* core) : QMainWindow(0), m_core(core) {
    m_docking_manager = new ToolWindowManager;

    int id = QFontDatabase::addApplicationFont(QStringLiteral("data/fonts/DejaVuSansMono.ttf"));
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    qDebug() << family;

    m_playlist_model = new PlaylistModel(this);
    //m_playlist_model->setHeaderData(0, Qt::Horizontal, QStringLiteral("Title"));
    //m_playlist_model->setHeaderData(1, Qt::Horizontal, QStringLiteral("Duration"));
    //m_playlist_model->setHeaderData(2, Qt::Horizontal, QStringLiteral("Information"));

    m_general_messages = HippoServiceAPI_get_message_api(hippo_service_api_new(m_core), HIPPO_MESSAGE_API_VERSION);

    QVBoxLayout* layout = new QVBoxLayout;
    QWidget* main_widget = new QWidget;

    main_widget->setLayout(layout);
    setCentralWidget(main_widget);

    layout->QLayout::addWidget(m_docking_manager);

    // Update code for processing the message queue
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update_messages);
    timer->start(5);

    /*
        // Create example content label - this can be any application specific
        // widget
        QLabel* l = new QLabel();
        l->setWordWrap(true);
        l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        l->setText(QStringLiteral("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. "));

        // Create a dock widget with the title Label 1 and set the created label
        // as the dock widget content
        ads::CDockWidget* DockWidget = new ads::CDockWidget(QStringLiteral("Label 1"));
        DockWidget->setWidget(l);

        // Add the toggleViewAction of the dock widget to the menu to give
        // the user the possibility to show the dock widget if it has been closed
        //ui->menuView->addAction(DockWidget->toggleViewAction());

        // Add the dock widget to the top dock widget area
        m_docking_manager->addDockWidget(ads::TopDockWidgetArea, DockWidget);
        */

    /*
    QVBoxLayout* layout = new QVBoxLayout;

    main_widget->setLayout(layout);
    main_widget->resize(1200, 1000);

    m_docking_manager->setParent(main_widget);
    layout->addWidget(m_docking_manager);
    */

    // resize(800, 600);

    create_menus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This gets the message APIs to send down to the backend to process the messages there. Notice that we have special
// messages queue at 0 which is sent from "non-plugins" such as adding files, etc. So in order to deal with that
// we check for 0 and return that queue, otherwise we offset by 1 and return the view plugins

const HippoMessageAPI* MainWindow::get_messages(void* this_, int index) {
    MainWindow* main_win = (MainWindow*)this_;

    if (index == 0) {
        return main_win->m_general_messages;
    } else {
        return HippoServiceAPI_get_message_api(main_win->m_plugin_instances[index - 1].service_api,
                                               HIPPO_MESSAGE_API_VERSION);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::handle_incoming_messages(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    switch (message->message_type())
	{
    	case MessageType_reply_added_urls:
		{
			auto urls = message->message_as_reply_added_urls()->urls();

			for (int i = 0, e = urls->Length(); i < e; ++i) {
			    auto url = urls->Get(i);
				//auto path = url->path();
				auto title = url->title();

				printf("added urls\n");

				m_playlist_model->add_entry(QString::fromUtf8(title->c_str(), title->size()),
				    QStringLiteral("Unknown"), QStringLiteral(""));

				m_playlist_model->layoutChanged();

                // Get the position
                //int row = m_playlist_model->rowCount();

                // Enable add one or more rows
                //m_playlist_model->insertRows(row, 1);

                // Get the row for Edit mode
                /*
                QModelIndex index = m_playlist_model->index(row);

                m_playlist_model->setData(index, QString::fromUtf8(title->c_str(), title->size()), Qt::EditRole);
                m_playlist_model->setData(index, QByteArray(path->c_str(), path->size()), Qt::UserRole);
                */
			}

			break;
		}

		default: break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::send_messages_to_ui(void* this_, const unsigned char* data, int len, int index) {
    MainWindow* main_win = (MainWindow*)this_;

    // We ignore message to the general queue right now
    if (index == 0) {
        main_win->handle_incoming_messages(data, index);
        return;
    }

    main_win->m_plugin_instances[index - 1].view_plugin->event(data, len);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::update_messages() {
    hippo_update_messages(m_core, this, m_plugin_instances.count() + 1, get_messages, send_messages_to_ui);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::create_menus() {
    QMenu* file_menu = menuBar()->addMenu(QStringLiteral("&Playlist"));

    QAction* add_files = new QAction(tr("&Add files.."), this);
    add_files->setShortcut(QKeySequence(QStringLiteral("Ctrl+A")));
    add_files->setStatusTip(tr("Add file(s)/link(s) to the playlist"));
    file_menu->addAction(add_files);

    QAction* remove_playlist_entry = new QAction(tr("&Remove selected"), this);
    remove_playlist_entry->setShortcut(QKeySequence::Delete);
    add_files->setStatusTip(tr("Remove selected items in the playlist"));
    file_menu->addAction(remove_playlist_entry);

    connect(add_files, &QAction::triggered, this, &MainWindow::add_files);
    connect(remove_playlist_entry, &QAction::triggered, this, &MainWindow::remove_playlist_entry);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::add_files() {
	// Using native dialog will crash/hang on Windows due to COMInit issue
    QStringList filenames = QFileDialog::getOpenFileNames(this, QStringLiteral("Add files files"), QDir::currentPath(),
                                                          QStringLiteral("All files (*.*)"), nullptr, QFileDialog::DontUseNativeDialog);
    if (filenames.isEmpty()) {
        return;
    }

    std::vector<flatbuffers::Offset<flatbuffers::String>> urls;
    flatbuffers::FlatBufferBuilder builder(4096);

    urls.reserve(filenames.length());

    for (auto& filename : filenames) {
        QByteArray filename_data = filename.toUtf8();
        const char* filename_utf8 = filename_data.constData();

        urls.push_back(builder.CreateString(filename_utf8));

        qDebug() << filename_utf8;
    }

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_add_urls,
        CreateHippoRequestAddUrls(builder, builder.CreateVector(urls)).Union()));
    HippoMessageAPI_send(m_general_messages, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::remove_playlist_entry() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MainWindow::load_plugins(const QString& plugin_dir) {
    QDir dir(plugin_dir);
    int plugins_found = 0;

#if defined(Q_OS_WIN)
    QString shared_extension = QStringLiteral(".dll");
#elif defined(Q_OS_MAC)
    QString shared_extension = QStringLiteral(".dylib");
#else
    QString shared_extension = QStringLiteral(".so");
#endif
    QString core_name = QStringLiteral("hippo_core");

    const QStringList entries = dir.entryList(QDir::Files);
    for (const QString& filename : entries) {
        // if filename doesn't contain lib extension or is core lib we skip
        if (!filename.endsWith(&shared_extension, Qt::CaseSensitive) || filename.contains(&core_name)) {
            continue;
        }

        QPluginLoader* plugin_loader = new QPluginLoader(dir.absoluteFilePath(filename));
        QJsonObject meta_data = plugin_loader->metaData();

        QJsonValue plugin_name_obj = meta_data.value(QStringLiteral("MetaData"));

        if (plugin_name_obj == QJsonValue::Undefined) {
            delete plugin_loader;
            continue;
        }

        plugin_name_obj = plugin_name_obj.toObject().value(QStringLiteral("hippo_view_plugin_name"));

        qDebug() << "Found plugin " << plugin_name_obj.toString();

        MainWindow::PluginInfo plugin_info = {
            plugin_loader,
            plugin_name_obj.toString(),
        };

        m_plugin_types.append(plugin_info);

        plugins_found++;
    }

    return plugins_found > 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* MainWindow::create_plugin_by_index(int index) {
    if (index >= m_plugin_types.length()) {
        return nullptr;
    }

    // Create new plugin instance
    QObject* plugin_obj = m_plugin_types[index].plugin->instance();

    if (!plugin_obj) {
        qDebug() << "Unable to create plugin " << m_plugin_types[index].plugin_name;
        return nullptr;
    }

    HippoQtView* view_plugin = qobject_cast<HippoQtView*>(plugin_obj);

    if (!view_plugin) {
        qDebug() << "Unable to cast plugin " << m_plugin_types[index].plugin_name;
        return nullptr;
    }

    HippoServiceAPI* service_api = hippo_service_api_new(m_core);
    QWidget* widget = view_plugin->create(service_api, m_playlist_model);

    m_plugin_instances.append({view_plugin, service_api, widget});

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Save layouts and such here also

void MainWindow::closeEvent(QCloseEvent *event) {
	hippo_core_drop(m_core);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::setup_default_plugins() {
	// TODO: Use names instead of index
	QWidget* player = create_plugin_by_index(0);
	QWidget* playlist = create_plugin_by_index(1);
	QWidget* tracker = create_plugin_by_index(2);

	m_docking_manager->addToolWindow(player, ToolWindowManager::EmptySpace);

	m_docking_manager->addToolWindow(tracker,
			ToolWindowManager::AreaReference(ToolWindowManager::RightOf, m_docking_manager->areaOf(player)));

	m_docking_manager->addToolWindow(playlist,
			ToolWindowManager::AreaReference(ToolWindowManager::BottomOf, m_docking_manager->areaOf(player)));
}
