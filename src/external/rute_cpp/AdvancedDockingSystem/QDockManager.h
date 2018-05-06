#pragma once

#include "DockManager.h"

class QDockManager : public ads::CDockManager {
public:
  QDockManager (QWidget *parent = 0) : ads::CDockManager(parent) {}
};


