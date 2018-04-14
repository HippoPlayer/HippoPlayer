#pragma once

#include "ToolWindowManager.h"

class QToolWindowManager : public ToolWindowManager {
public:
  QToolWindowManager(QWidget *parent = 0) : ToolWindowManager(parent) {}
};


