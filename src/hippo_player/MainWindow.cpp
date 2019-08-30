#include "MainWindow.h"
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QPluginLoader>
#include "../../src/plugin_api/HippoQtView.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() {}

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
    return view_plugin->create();
}

