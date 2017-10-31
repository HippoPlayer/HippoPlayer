
#include "../../include/wrui.h"
#include <QMainWindow>

/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void add_dock_widget(GUMainWindow* win, GUDockingArea area, struct GUDockWidget* w) {
	QMainWindow* main_win = (QMainWindow*)win->base->object->p;
	QDockWidget* widget = (QDockWidget*)w->base->object->p;

	int flags = 0;

	if (area & GUDockingArea_Left)
	    flags |= Qt::LeftDockWidgetArea;

	if (area & GUDockingArea_Right)
	    flags |= Qt::RightDockWidgetArea;

	if (area & GUDockingArea_Top)
	    flags |= Qt::TopDockWidgetArea;

	if (area & GUDockingArea_Bottom)
	    flags |= Qt::BottomDockWidgetArea;

    return main_win->addDockWidget((Qt::DockWidgetArea)flags, widget);;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GUMainWindowFuncs g_mainWindowFuncs = {
	add_dock_widget,
};
*/



