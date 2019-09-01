#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include "MainWindow.h"
#include "src/hippo_core_loader/hippo_core_loader.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Make sure we manage to load the core
    if (!HippoCore_load()) {
        return 1;
    }

    QFile f(QStringLiteral("bin/player/themes/dark/style.qss"));

    if (!f.exists()) {
        printf("Unable to set stylesheet\n");
        return 0;
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        app.setStyleSheet(ts.readAll());
    }

    MainWindow main_window;
    main_window.load_plugins(app.applicationDirPath());

    main_window.create_plugin_by_index(0);
    main_window.create_plugin_by_index(0);

    main_window.show();

    return app.exec();
}
