#pragma once

#include "DockWidget.h"

class QDockWidget : public ads::CDockWidget {
public:
  QDockWidget(QWidget* parent = 0) : ads::CDockWidget(QStringLiteral("Dummy"), parent) {}
};


