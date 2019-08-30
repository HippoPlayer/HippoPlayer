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

    /*
    HippoCore* hippo_core = hippo_core_new();
    printf("hippo_core %p\n", hippo_core);

    hippo_play_file(hippo_core, "/home/emoon/Downloads/musiklinjen.mod");
    */

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

    MainWindow* main_window = new MainWindow();

    main_window->load_plugins(app.applicationDirPath());

    QWidget* plugin_view = main_window->create_plugin_by_index(0);

    if (plugin_view) {
        plugin_view->setParent(main_window);
    }

    QMenu* file_menu = new QMenu();
    file_menu->setTitle(QStringLiteral("File menu"));

    main_window->menuBar()->addMenu(file_menu);
    main_window->show();

    /*
        self.app.set_style_sheet("bin/player/themes/dark/style.qss");

        let main_window = self.ui.create_main_window();
        let timer = self.ui.create_timer();

        let layout = self.ui.create_v_box_layout();

        let add_files = self.ui.create_action();
        add_files.set_shortcut_mod(Keys::KeyO, MetaKeys::Ctrl);
        add_files.set_text("Add Files");

        let file_menu = self.ui.create_menu();
        file_menu.set_title("File");
        file_menu.add_action(&add_files);

        let plugin_menu = self.create_plugins_menu();

        let menu_bar = main_window.menu_bar();
        menu_bar.add_menu(&file_menu);
        menu_bar.add_menu(&plugin_menu);

        set_timer_timeout_event!(timer, self, HippoPlayer,
       HippoPlayer::update_ui_messages); set_action_triggered_event!(add_files,
       self, HippoPlayer, HippoPlayer::add_files);
        set_application_about_to_quit_event!(self.app, self, HippoPlayer,
       HippoPlayer::before_quit);

        timer.start(5);

        self.main_widget.set_layout(&layout);
        self.main_widget.resize(1200, 1000);

        main_window.set_central_widget(&self.main_widget);

        self.dock_manager.set_parent(&self.main_widget);

        layout.add_widget(&self.dock_manager);

        //player_window.set_content(&main_window);
        //player_window.set_window_title(&format!("HippoPlayer 0.0.1 {}",
       build_id()));

        main_window.resize(800, 600);

        self.load_layout("layout.data");
        self.load_playlist("playlist.json");

        main_window.show();
    */

    return app.exec();
}
