#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyleFactory>
#include "MainWindow.h"
#include "src/hippo_core_loader/hippo_core_loader.h"

extern "C" {
	#include "src/hippo_core/native/hippo_core.h"
}

int main(int argc, char** argv) {

    // Make sure we manage to load the core
    if (!HippoCore_load()) {
        return 1;
    }

    HippoCore* core = hippo_core_new();

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    app.setPalette(darkPalette);
    app.setStyleSheet(QStringLiteral("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"));

    MainWindow main_window(core);
    main_window.load_plugins(app.applicationDirPath());

    main_window.create_plugin_by_index(0);
    main_window.create_plugin_by_index(1);

    main_window.show();

    return app.exec();
}
