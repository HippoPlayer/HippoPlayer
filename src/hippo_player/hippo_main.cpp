#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include "MainWindow.h"
#include "src/hippo_core/native/hippo_core.h"
#include "src/hippo_core_loader/hippo_core_loader.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Make sure we manage to load the core
    if (!HippoCore_load()) {
        return 1;
    }

    HippoCore* hippo_core = hippo_core_new();

    //hippo_play_file(hippo_core, "/home/emoon/Downloads/musiklinjen.mod");

    QFile f(QStringLiteral("bin/player/themes/dark/style.qss"));

    if (!f.exists()) {
        printf("Unable to set stylesheet\n");
        return 0;
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        app.setStyleSheet(ts.readAll());
    }

    // QPushButton button(QStringLiteral("Hello world !"));
    // button.show();

    MainWindow main_window;

    main_window.load_plugins(app.applicationDirPath());

    main_window.create_plugin_by_index(0);
    main_window.create_plugin_by_index(0);

    /*
    QMenu* file_menu = new QMenu();
    file_menu->setTitle(QStringLiteral("File menu"));

    main_window->menuBar()->addMenu(file_menu);
    */
    main_window.show();

    return app.exec();
}
