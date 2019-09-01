#pragma once

#include <QtCore/QPluginLoader>
#include <QtCore/QVector>
#include <QtWidgets/QMainWindow>

class ToolWindowManager;
class HippoQtView;
struct HippoServiceAPI;
struct HippoCore;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    bool load_plugins(const QString& plugin_dir);
    QWidget* create_plugin_by_index(int index);

private:
    struct PluginInfo {
        QPluginLoader* plugin;
        QString plugin_name;
    };

    struct PluginInstance {
        HippoQtView* view_plugin;
        HippoServiceAPI* service_api;
        QWidget* widget;
    };

    HippoCore* m_core = nullptr;
    ToolWindowManager* m_docking_manager = nullptr;
    QVector<PluginInfo> m_plugin_types;
    QVector<PluginInstance> m_plugin_instances;
};
