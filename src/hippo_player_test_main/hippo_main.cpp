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

    HippoCore* hippo_core = hippo_core_new();
    printf("hippo_core %p\n", hippo_core);

    hippo_play_file(hippo_core, "/home/emoon/Downloads/musiklinjen.mod");

    QPushButton button(QStringLiteral("Hello world !"));
    button.show();

    return app.exec();
}
