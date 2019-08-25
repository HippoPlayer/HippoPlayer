#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include "src/hippo_core_loader/hippo_core_loader.h"
#include "src/hippo_core/native/hippo_core.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Make sure we manage to load the core
    if (!HippoCore_load()) {
        return 1;
    }

    struct SongDb* db = hippo_song_db_new();
    printf("db %p\n", db);

    hippo_song_db_update(db, "test");
    hippo_song_db_update(db, "test");

    QPushButton button(QStringLiteral("Hello world !"));
    button.show();

    return app.exec();
}
