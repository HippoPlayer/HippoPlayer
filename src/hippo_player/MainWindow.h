#pragma once

#include <QtCore/QPluginLoader>
#include <QtCore/QVector>
#include <QtWidgets/QMainWindow>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ads {
    class CDockManager;
}

namespace Ui {
    class MainWindow;
}

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

    Ui::MainWindow* ui;
    ads::CDockManager* m_docking_manager = nullptr;
    QVector<PluginInfo> m_plugin_types;
};
