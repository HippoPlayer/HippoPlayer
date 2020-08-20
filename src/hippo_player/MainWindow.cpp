#include "MainWindow.h"
#include <flatbuffers/flatbuffers.h>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QPluginLoader>
#include <QtCore/QStringListModel>
#include <QtCore/QTimer>
#include <QtGui/QFontDatabase>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenuBar>
#include "../../src/plugin_api/HippoMessages.h"
#include "../../src/plugin_api/HippoPlugin.h"
#include "../../src/plugin_api/HippoQtView.h"
#include "PlaylistModel.h"
#include "PrefsDialog.h"

#include "src/external/toolwindowmanager/src/ToolWindowManager.h"
extern "C" {
#include "src/hippo_core/native/hippo_core.h"
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(HippoCore* core) : QMainWindow(0), m_core(core) {
    m_docking_manager = new ToolWindowManager;

    QString window_title = QStringLiteral("HippoPlayer ");
    window_title += QString::fromUtf8(HIPPO_VERSION);

    setWindowTitle(window_title);

    // int id = QFontDatabase::addApplicationFont(QStringLiteral("data/fonts/DejaVuSansMono.ttf"));
    // QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    m_playlist_model = new PlaylistModel(m_core, this);
    // m_playlist_model->setSelectionMode(QAbstractItemView::MultiSelection);

    m_general_messages = HippoServiceAPI_get_message_api(hippo_service_api_new(m_core), HIPPO_MESSAGE_API_VERSION);

    QVBoxLayout* layout = new QVBoxLayout;
    QWidget* main_widget = new QWidget;

    main_widget->setLayout(layout);
    setCentralWidget(main_widget);

    layout->QLayout::addWidget(m_docking_manager);

    printf("general messeages %p\n", m_general_messages);

    m_console = new ConsoleView(m_general_messages, nullptr);
    m_prefs_dialog = new PrefsDialog(m_general_messages);

    m_docking_manager->addToolWindow(m_console, ToolWindowManager::NewFloatingArea,
                                     ToolWindowManager::ToolWindowProperty::HideOnClose);

    m_docking_manager->hideToolWindow(m_console);

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

    main_widget->resize(800, 600);

    create_menus();

    // resize(2840, 1600);
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
    // let console handle messages
    m_console->incoming_messages(data, len);
    m_prefs_dialog->incoming_messages(data, len);

    const HippoMessage* message = GetHippoMessage(data);

    switch (message->message_type()) {
        case MessageType_reply_added_urls: {
            auto reply_msg = message->message_as_reply_added_urls();
            auto urls = reply_msg->urls();

            m_playlist_model->begin_insert(reply_msg->index(), 0);

            for (int i = 0, e = urls->Length(); i < e; ++i) {
                auto url = urls->Get(i);
                m_playlist_model->insert_entry(url);
            }

            m_playlist_model->end_insert();

            m_playlist_model->layoutChanged();

            break;
        }

        case MessageType_select_song: {
            auto msg = message->message_as_select_song();
            m_playlist_model->update_index(msg);
            break;
        }

        default:
            break;
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
    if (m_update_events) {
        hippo_update_messages(m_core, this, m_plugin_instances.count() + 1, get_messages, send_messages_to_ui);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::create_menus() {
    QMenu* file_menu = menuBar()->addMenu(QStringLiteral("&Playlist"));

    QAction* add_files = new QAction(tr("&Add files.."), this);
    add_files->setStatusTip(tr("Add file(s)/link(s) to the playlist"));
    file_menu->addAction(add_files);

    QAction* add_dir = new QAction(tr("&Add Directory.."), this);
    add_dir->setShortcut(QKeySequence(QStringLiteral("Ctrl+D")));
    add_dir->setStatusTip(tr("Add directory of files to the playlist"));
    file_menu->addAction(add_dir);

    // Remove playlist entry

    QAction* remove_playlist_entry = new QAction(tr("&Remove selected"), this);
    remove_playlist_entry->setShortcut(QKeySequence::Delete);
    add_files->setStatusTip(tr("Remove selected items in the playlist"));
    file_menu->addAction(remove_playlist_entry);

    file_menu->addSeparator();

    // Prefs

    QAction* prefs_entry = new QAction(tr("&Preferences"), this);
    prefs_entry->setShortcut(QKeySequence(QStringLiteral("Ctrl+P")));
    prefs_entry->setStatusTip(tr("Show Preferences Dialog"));
    file_menu->addAction(prefs_entry);
    QObject::connect(prefs_entry, &QAction::triggered, this, &MainWindow::show_prefs);

    QAction* exit_act = new QAction(tr("E&xit"), this);
    exit_act->setShortcuts(QKeySequence::Quit);
    exit_act->setStatusTip(tr("Exit the application"));
    connect(exit_act, &QAction::triggered, this, &QWidget::close);
    file_menu->addAction(exit_act);

    connect(add_files, &QAction::triggered, this, &MainWindow::add_files);
    connect(add_dir, &QAction::triggered, this, &MainWindow::add_directory);

    // Add console to the menu
    QMenu* view_menu = menuBar()->addMenu(QStringLiteral("&View"));
    QAction* console_view_act = new QAction(tr("Console"), this);
    console_view_act->setStatusTip(tr("Show Console with log messages"));
    QObject::connect(console_view_act, &QAction::triggered, this, &MainWindow::show_hide_console);
    view_menu->addAction(console_view_act);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::add_directory() {
    // Using native dialog will crash/hang on Windows due to COMInit issue
    QString dir_name = QFileDialog::getExistingDirectory(
        this, tr("Select Directory"), tr(""),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog);

    if (dir_name.isEmpty()) {
        return;
    }

    // TODO: Spaw thread for this
    std::vector<flatbuffers::Offset<flatbuffers::String>> urls;
    flatbuffers::FlatBufferBuilder builder(4096);

    urls.reserve(1024);

    QDirIterator it(dir_name, QStringList() << tr("*.*"), QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QByteArray filename_data = it.next().toUtf8();
        const char* filename_utf8 = filename_data.constData();

        urls.push_back(builder.CreateString(filename_utf8));
    }

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_add_urls,
                                            CreateHippoRequestAddUrls(builder, builder.CreateVector(urls)).Union()));
    HippoMessageAPI_send(m_general_messages, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::add_files() {
    // Using native dialog will crash/hang on Windows due to COMInit issue
    QStringList filenames =
        QFileDialog::getOpenFileNames(this, QStringLiteral("Add files files"), QDir::currentPath(),
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

void MainWindow::show_hide_console() {
    m_docking_manager->moveToolWindow(m_console, ToolWindowManager::NewFloatingArea);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::show_prefs() {
    m_prefs_dialog->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::create_plugin_menus() {
    QMenu* plugin_menu = menuBar()->addMenu(QStringLiteral("&Plugins"));
    int plugin_index = 0;

    for (auto& plugin : m_plugin_types) {
        QAction* plugin_menu_entry = new QAction(plugin.plugin_name);
        plugin_menu_entry->setData(plugin_index);
        plugin_menu->addAction(plugin_menu_entry);

        connect(plugin_menu_entry, &QAction::triggered, this,
                [this, plugin_index]() { create_plugin_instance(plugin_index); });
        plugin_index++;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::create_plugin_instance(int index) {
    QWidget* plugin_widget = create_plugin_by_index(index);

    if (plugin_widget) {
        m_docking_manager->addToolWindow(plugin_widget, ToolWindowManager::LastUsedArea);
    }
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

        // qDebug() << "Found plugin " << plugin_name_obj.toString();

        MainWindow::PluginInfo plugin_info = {
            plugin_loader,
            plugin_name_obj.toString(),
        };

        m_plugin_types.append(plugin_info);

        plugins_found++;
    }

    create_plugin_menus();

    return plugins_found > 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* MainWindow::create_plugin_by_name(const QString& plugin_name) {
    for (int i = 0, e = m_plugin_types.length(); i < e; ++i) {
        if (m_plugin_types[i].plugin_name == plugin_name) {
            return create_plugin_by_index(i);
        }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::plugin_view_closed(QObject* obj) {
    for (size_t i = 0, len = m_plugin_instances.size(); i < len; ++i) {
        if (m_plugin_instances[i].widget == obj) {
            m_plugin_instances.remove(i);
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* MainWindow::create_plugin_by_index(int index) {
    if (index >= m_plugin_types.length()) {
        return nullptr;
    }

    // Create new plugin instance
    QObject* plugin_obj = m_plugin_types[index].plugin->instance();

    if (!plugin_obj) {
        qDebug() << "Unable to create plugin " << m_plugin_types[index].plugin_name << " "
                 << m_plugin_types[index].plugin->errorString();
        return nullptr;
    }

    HippoQtView* view_plugin = qobject_cast<HippoQtView*>(plugin_obj);

    if (!view_plugin) {
        qDebug() << "Unable to cast plugin " << m_plugin_types[index].plugin_name;
        return nullptr;
    }

    HippoServiceAPI* service_api = hippo_service_api_new(m_core);
    QWidget* widget = view_plugin->create(service_api, m_playlist_model);
    widget->setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(widget, &QObject::destroyed, this, &MainWindow::plugin_view_closed);

    m_plugin_instances.append({view_plugin, service_api, widget});

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Save layouts and such here also

void MainWindow::closeEvent(QCloseEvent* event) {
    m_update_events = false;

    delete m_console;
    m_console = nullptr;

    // disconnect all the events so we don't get them when MainWindow is destroyed
    for (const auto& inst : m_plugin_instances) {
        QObject::disconnect(inst.widget, &QObject::destroyed, this, &MainWindow::plugin_view_closed);
    }

    m_plugin_instances.clear();
    hippo_core_drop(m_core);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::setup_default_plugins() {
    QWidget* player = create_plugin_by_name(QStringLiteral("Player"));
    QWidget* playlist = create_plugin_by_name(QStringLiteral("Playlist"));
    QWidget* music_info = create_plugin_by_name(QStringLiteral("Music Info"));

    m_docking_manager->addToolWindow(player, ToolWindowManager::EmptySpace);

    m_docking_manager->addToolWindow(
        music_info, ToolWindowManager::AreaReference(ToolWindowManager::RightOf, m_docking_manager->areaOf(player)));

    m_docking_manager->addToolWindow(
        playlist, ToolWindowManager::AreaReference(ToolWindowManager::BottomOf, m_docking_manager->areaOf(player)));
}
