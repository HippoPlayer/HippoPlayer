#include "player.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* PlayerView::create() {
    QWidget* widget = new QWidget;

    QPushButton* play_button = new QPushButton(widget);

    //
    play_button->setText(QStringLiteral("PlayerView"));

    return widget;
}

