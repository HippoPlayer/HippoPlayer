#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QPushButton button(QStringLiteral("Hello world !"));
    button.show();

    return app.exec();
}
