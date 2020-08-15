#pragma once

#include <QtCore/QPluginLoader>
#include <QtCore/QVector>
#include <QtWidgets/QMainWindow>
#include "console.h"

class QStringListModel;
class ToolWindowManager;
class HippoQtView;
class PlaylistModel;
struct HippoServiceAPI;
struct HippoCore;
struct HippoMessageAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(HippoCore* core);
    bool load_plugins(const QString& plugin_dir);
    void setup_default_plugins();
    QWidget* create_plugin_by_name(const QString& plugin_name);
    QWidget* create_plugin_by_index(int index);
	void closeEvent(QCloseEvent* event);

private:
    // Callback into the core to get internal messages
    static const HippoMessageAPI* get_messages(void* this_, int index);
    static void send_messages_to_ui(void* this_, const unsigned char* data, int len, int index);
    void handle_incoming_messages(const unsigned char* data, int len);
    void create_plugin_menus();

    void create_menus();

    struct PluginInfo {
        QPluginLoader* plugin;
        QString plugin_name;
    };

    struct PluginInstance {
        HippoQtView* view_plugin;
        HippoServiceAPI* service_api;
        QWidget* widget;
    };

    Q_SLOT void plugin_view_closed(QObject* obj);
    Q_SLOT void show_hide_console();

    PlaylistModel* m_playlist_model;
    HippoCore* m_core = nullptr;
    const HippoMessageAPI* m_general_messages = nullptr;
    ToolWindowManager* m_docking_manager = nullptr;
    QVector<PluginInfo> m_plugin_types;
    QVector<PluginInstance> m_plugin_instances;
    ConsoleView* m_console = nullptr;
    bool m_console_visible = false;

    void create_plugin_instance(int index);
    Q_SLOT void add_files();
    Q_SLOT void add_directory();
    //Q_SLOT void remove_playlist_entry();
    Q_SLOT void update_messages();
};
