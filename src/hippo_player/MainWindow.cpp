#include "MainWindow.h"
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QPluginLoader>
#include <QtWidgets/QBoxLayout>
#include "../../src/plugin_api/HippoQtView.h"
#include "src/external/rute_cpp/AdvancedDockingSystem/QDockManager.h"
#include "src/external/rute_cpp/AdvancedDockingSystem/QDockWidget.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() {
    m_docking_manager = new QDockManager();

    QVBoxLayout* layout = new QVBoxLayout;
    QWidget* main_widget = new QWidget;

    main_widget->setLayout(layout);
    main_widget->resize(1200, 1000);

    setCentralWidget(main_widget);

    m_docking_manager->setParent(main_widget);
    layout->addWidget(m_docking_manager);

    resize(800, 600);
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
        if (!filename.endsWith(&shared_extension, Qt::CaseSensitive) ||
            filename.contains(&core_name)) {
            continue;
        }

        QPluginLoader* plugin_loader = new QPluginLoader(dir.absoluteFilePath(filename));
        QJsonObject meta_data = plugin_loader->metaData();

        QJsonValue plugin_name_obj =
            meta_data.value(QStringLiteral("MetaData"));

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

    printf("created plugin");

    // TODO: Store instance
    QWidget* plugin_view = view_plugin->create();
    QDockWidget* dock_widget = new QDockWidget;

    dock_widget->setWidget(plugin_view);

    m_docking_manager->addDockWidget(DockWidgetArea::test, dock_widget);

    return nullptr;
}

