#pragma once

#include <QtWidgets/QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class QPaintEvent;

class TrackerDisplay : public QWidget {
public:
	TrackerDisplay(QWidget* parent);

private:

	void paintEvent(QPaintEvent* event);
};
