#pragma once

#include <QtCore/QPluginLoader>
#include <QtCore/QVector>
#include <QtWidgets/QMainWindow>

class ToolWindowManager;
class HippoQtView;
struct HippoServiceAPI;
struct HippoCore;
struct HippoMessageDecode;
struct HippoMessageAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    bool load_plugins(const QString& plugin_dir);
    QWidget* create_plugin_by_index(int index);

private:
    // Callback into the core to get internal messages
    static const HippoMessageAPI* get_messages(void* this_, int index);
    static void send_messages_to_ui(void* this_, const HippoMessageDecode* msg, int index);

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


    const HippoMessageAPI* m_general_messages = nullptr;
    HippoCore* m_core = nullptr;
    ToolWindowManager* m_docking_manager = nullptr;
    QVector<PluginInfo> m_plugin_types;
    QVector<PluginInstance> m_plugin_instances;

    Q_SLOT void add_files();
    Q_SLOT void update_messages();
};
