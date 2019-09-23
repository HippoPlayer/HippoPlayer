#include "tracker_display.h"
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TrackerDisplay::TrackerDisplay(QWidget* parent) : QWidget(parent)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	painter.setBrush(Qt::blue);
	painter.drawRect(geometry());
}

