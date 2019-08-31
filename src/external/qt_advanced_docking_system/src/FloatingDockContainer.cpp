/*******************************************************************************
 ** Qt Advanced Docking System
 ** Copyright (C) 2017 Uwe Kindler
 **
 ** This library is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2.1 of the License, or (at your option) any later version.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

//============================================================================
/// \file   FloatingDockContainer.cpp
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Implementation of CFloatingDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingDockContainer.h"

#include <QBoxLayout>
#include <QApplication>
#include <QMouseEvent>
#include <QPointer>
#include <QAction>
#include <QDebug>
#include <QAbstractButton>
#include <QElapsedTimer>

#include "DockContainerWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockOverlay.h"

#ifdef Q_OS_LINUX
#include "linux/FloatingWidgetTitleBar.h"
#include <xcb/xcb.h>
#endif

namespace ads
{
static unsigned int zOrderCounter = 0;
/**
 * Private data class of CFloatingDockContainer class (pimpl)
 */
struct FloatingDockContainerPrivate
{
	CFloatingDockContainer *_this;
	CDockContainerWidget *DockContainer;
	unsigned int zOrderIndex = ++zOrderCounter;
	QPointer<CDockManager> DockManager;
	eDragState DraggingState = DraggingInactive;
	QPoint DragStartMousePosition;
	CDockContainerWidget *DropContainer = nullptr;
	CDockAreaWidget *SingleDockArea = nullptr;
#ifdef Q_OS_LINUX
    QWidget* MouseEventHandler = nullptr;
    CFloatingWidgetTitleBar* TitleBar = nullptr;
#endif

	/**
	 * Private data constructor
	 */
	FloatingDockContainerPrivate(CFloatingDockContainer *_public);

	void titleMouseReleaseEvent();
	void updateDropOverlays(const QPoint &GlobalPos);

	/**
	 * Tests is a certain state is active
	 */
	bool isState(eDragState StateId) const
	{
		return StateId == DraggingState;
	}

	void setState(eDragState StateId)
	{
		DraggingState = StateId;
	}

	void setWindowTitle(const QString &Text)
	{
#ifdef Q_OS_LINUX
        TitleBar->setTitle(Text);
#else
		_this->setWindowTitle(Text);
#endif
	}
};
// struct FloatingDockContainerPrivate

//============================================================================
FloatingDockContainerPrivate::FloatingDockContainerPrivate(
    CFloatingDockContainer *_public) :
	_this(_public)
{

}

//============================================================================
void FloatingDockContainerPrivate::titleMouseReleaseEvent()
{
	setState(DraggingInactive);
	if (!DropContainer)
	{
		return;
	}

	if (DockManager->dockAreaOverlay()->dropAreaUnderCursor()
	    != InvalidDockWidgetArea
	    || DockManager->containerOverlay()->dropAreaUnderCursor()
	        != InvalidDockWidgetArea)
	{
		// Resize the floating widget to the size of the highlighted drop area
		// rectangle
		CDockOverlay *Overlay = DockManager->containerOverlay();
		if (!Overlay->dropOverlayRect().isValid())
		{
			Overlay = DockManager->dockAreaOverlay();
		}

		QRect Rect = Overlay->dropOverlayRect();
		int FrameWidth = (_this->frameSize().width() - _this->rect().width())
		    / 2;
		int TitleBarHeight = _this->frameSize().height()
		    - _this->rect().height() - FrameWidth;
		if (Rect.isValid())
		{
			QPoint TopLeft = Overlay->mapToGlobal(Rect.topLeft());
			TopLeft.ry() += TitleBarHeight;
			_this->setGeometry(
			    QRect(TopLeft,
			        QSize(Rect.width(), Rect.height() - TitleBarHeight)));
			QApplication::processEvents();
		}
		DropContainer->dropFloatingWidget(_this, QCursor::pos());
	}

	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
}

//============================================================================
void FloatingDockContainerPrivate::updateDropOverlays(const QPoint &GlobalPos)
{
	if (!_this->isVisible() || !DockManager)
	{
		return;
	}

	auto Containers = DockManager->dockContainers();
	CDockContainerWidget *TopContainer = nullptr;
	for (auto ContainerWidget : Containers)
	{
		if (!ContainerWidget->isVisible())
		{
			continue;
		}

		if (DockContainer == ContainerWidget)
		{
			continue;
		}

		QPoint MappedPos = ContainerWidget->mapFromGlobal(GlobalPos);
		if (ContainerWidget->rect().contains(MappedPos))
		{
			if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
			{
				TopContainer = ContainerWidget;
			}
		}
	}

	DropContainer = TopContainer;
	auto ContainerOverlay = DockManager->containerOverlay();
	auto DockAreaOverlay = DockManager->dockAreaOverlay();

	if (!TopContainer)
	{
		ContainerOverlay->hideOverlay();
		DockAreaOverlay->hideOverlay();
		return;
	}

	int VisibleDockAreas = TopContainer->visibleDockAreaCount();
	ContainerOverlay->setAllowedAreas(
	    VisibleDockAreas > 1 ? OuterDockAreas : AllDockAreas);
	DockWidgetArea ContainerArea = ContainerOverlay->showOverlay(TopContainer);
	ContainerOverlay->enableDropPreview(ContainerArea != InvalidDockWidgetArea);
	auto DockArea = TopContainer->dockAreaAt(GlobalPos);
	if (DockArea && DockArea->isVisible() && VisibleDockAreas > 0)
	{
		DockAreaOverlay->enableDropPreview(true);
		DockAreaOverlay->setAllowedAreas(
		    (VisibleDockAreas == 1) ? NoDockWidgetArea : AllDockAreas);
		DockWidgetArea Area = DockAreaOverlay->showOverlay(DockArea);

		// A CenterDockWidgetArea for the dockAreaOverlay() indicates that
		// the mouse is in the title bar. If the ContainerArea is valid
		// then we ignore the dock area of the dockAreaOverlay() and disable
		// the drop preview
		if ((Area == CenterDockWidgetArea)
		    && (ContainerArea != InvalidDockWidgetArea))
		{
			DockAreaOverlay->enableDropPreview(false);
			ContainerOverlay->enableDropPreview(true);
		}
		else
		{
			ContainerOverlay->enableDropPreview(InvalidDockWidgetArea == Area);
		}
	}
	else
	{
		DockAreaOverlay->hideOverlay();
	}
}

//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockManager *DockManager) :
	tFloatingWidgetBase(DockManager),
	d(new FloatingDockContainerPrivate(this))
{
	d->DockManager = DockManager;
	d->DockContainer = new CDockContainerWidget(DockManager, this);
	connect(d->DockContainer, SIGNAL(dockAreasAdded()), this,
	    SLOT(onDockAreasAddedOrRemoved()));
	connect(d->DockContainer, SIGNAL(dockAreasRemoved()), this,
	    SLOT(onDockAreasAddedOrRemoved()));

#ifdef Q_OS_LINUX
    d->TitleBar = new CFloatingWidgetTitleBar(this);
    setWindowFlags(windowFlags() | Qt::Tool);
    QDockWidget::setWidget(d->DockContainer);
    QDockWidget::setFloating(true);
    QDockWidget::setFeatures(QDockWidget::AllDockWidgetFeatures);
    setTitleBarWidget(d->TitleBar);
    connect(d->TitleBar, SIGNAL(closeRequested()), SLOT(close()));
#else
	setWindowFlags(
	    Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);
	l->addWidget(d->DockContainer);
#endif

	DockManager->registerFloatingWidget(this);

	// We install an event filter to detect mouse release events because we
	// do not receive mouse release event if the floating widget is behind
	// the drop overlay cross
	qApp->installEventFilter(this);
}

//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockAreaWidget *DockArea) :
	CFloatingDockContainer(DockArea->dockManager())
{
	d->DockContainer->addDockArea(DockArea);
#ifdef Q_OS_LINUX
    d->TitleBar->enableCloseButton(isClosable());
#endif
}

//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockWidget *DockWidget) :
	CFloatingDockContainer(DockWidget->dockManager())
{
	d->DockContainer->addDockWidget(CenterDockWidgetArea, DockWidget);
#ifdef Q_OS_LINUX
    d->TitleBar->enableCloseButton(isClosable());
#endif
}

//============================================================================
CFloatingDockContainer::~CFloatingDockContainer()
{
	ADS_PRINT("~CFloatingDockContainer");
	if (d->DockManager)
	{
		d->DockManager->removeFloatingWidget(this);
	}
	delete d;
}

//============================================================================
CDockContainerWidget* CFloatingDockContainer::dockContainer() const
{
	return d->DockContainer;
}

//============================================================================
void CFloatingDockContainer::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
	if ((event->type() == QEvent::ActivationChange) && isActiveWindow())
	{
		ADS_PRINT("FloatingWidget::changeEvent QEvent::ActivationChange ");
		d->zOrderIndex = ++zOrderCounter;
		return;
	}
}

//============================================================================
void CFloatingDockContainer::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	switch (d->DraggingState)
	{
	case DraggingMousePressed:
		d->setState(DraggingFloatingWidget);
		d->updateDropOverlays(QCursor::pos());
		break;

	case DraggingFloatingWidget:
		d->updateDropOverlays(QCursor::pos());
		break;
	default:
		break;
	}
}

//============================================================================
void CFloatingDockContainer::closeEvent(QCloseEvent *event)
{
	ADS_PRINT("CFloatingDockContainer closeEvent");
	d->setState(DraggingInactive);

	if (isClosable())
	{
		// In Qt version after 5.9.2 there seems to be a bug that causes the
		// QWidget::event() function to not receive any NonClientArea mouse
		// events anymore after a close/show cycle. The bug is reported here:
		// https://bugreports.qt.io/browse/QTBUG-73295
		// The following code is a workaround for Qt versions > 5.9.2 that seems
		// to work
		// Starting from Qt version 5.12.2 this seems to work again. But
		// now the QEvent::NonClientAreaMouseButtonPress function returns always
		// Qt::RightButton even if the left button was pressed
#ifndef Q_OS_LINUX
#if (QT_VERSION > QT_VERSION_CHECK(5, 9, 2) && QT_VERSION < QT_VERSION_CHECK(5, 12, 2))
        event->ignore();
        this->hide();
#else
		Super::closeEvent(event);
#endif
#else // Q_OS_LINUX
        Super::closeEvent(event);
#endif
	}
	else
	{
		event->ignore();
	}
}

//============================================================================
void CFloatingDockContainer::hideEvent(QHideEvent *event)
{
	Super::hideEvent(event);
	for (auto DockArea : d->DockContainer->openedDockAreas())
	{
		for (auto DockWidget : DockArea->openedDockWidgets())
		{
			DockWidget->toggleView(false);
		}
	}
}

//============================================================================
void CFloatingDockContainer::showEvent(QShowEvent *event)
{
	Super::showEvent(event);
}

//============================================================================
bool CFloatingDockContainer::event(QEvent *e)
{
	switch (d->DraggingState)
	{
	case DraggingInactive:
	{
		// Normally we would check here, if the left mouse button is pressed.
		// But from QT version 5.12.2 on the mouse events from
		// QEvent::NonClientAreaMouseButtonPress return the wrong mouse button
		// The event always returns Qt::RightButton even if the left button
		// is clicked.
		// It is really great to work around the whole NonClientMouseArea
		// bugs
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 2))
		if (e->type()
		    == QEvent::NonClientAreaMouseButtonPress /*&& QGuiApplication::mouseButtons().testFlag(Qt::LeftButton)*/)
		{
			ADS_PRINT("FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type());
			d->setState(DraggingMousePressed);
		}
#else
		if (e->type() == QEvent::NonClientAreaMouseButtonPress && QGuiApplication::mouseButtons().testFlag(Qt::LeftButton))
		{
            ADS_PRINT("FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type());
			d->setState(DraggingMousePressed);
		}
#endif
	}
		break;

	case DraggingMousePressed:
		switch (e->type())
		{
		case QEvent::NonClientAreaMouseButtonDblClick:
			ADS_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonDblClick");
			d->setState(DraggingInactive);
			break;

		case QEvent::Resize:
			// If the first event after the mouse press is a resize event, then
			// the user resizes the window instead of dragging it around.
			// But there is one exception. If the window is maximized,
			// then dragging the window via title bar will cause the widget to
			// leave the maximized state. This in turn will trigger a resize event.
			// To know, if the resize event was triggered by user via moving a
			// corner of the window frame or if it was caused by a windows state
			// change, we check, if we are not in maximized state.
			if (!isMaximized())
			{
				d->setState(DraggingInactive);
			}
			break;

		default:
			break;
		}
		break;

	case DraggingFloatingWidget:
		if (e->type() == QEvent::NonClientAreaMouseButtonRelease)
		{
			ADS_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonRelease");
			d->titleMouseReleaseEvent();
		}
		break;

	default:
		break;
	}

#if (ADS_DEBUG_LEVEL > 0)
	qDebug() << "CFloatingDockContainer::event " << e->type();
#endif
	return QWidget::event(e);
}

//============================================================================
bool CFloatingDockContainer::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	if (event->type() == QEvent::MouseButtonRelease
	    && d->isState(DraggingFloatingWidget))
	{
		ADS_PRINT("FloatingWidget::eventFilter QEvent::MouseButtonRelease");
		finishDragging();
		d->titleMouseReleaseEvent();
	}

	return false;
}

//============================================================================
void CFloatingDockContainer::startFloating(const QPoint &DragStartMousePos,
    const QSize &Size, eDragState DragState, QWidget *MouseEventHandler)
{
#ifndef Q_OS_LINUX
	Q_UNUSED(MouseEventHandler)
#endif
	resize(Size);
	d->setState(DragState);
	d->DragStartMousePosition = DragStartMousePos;
#ifdef Q_OS_LINUX
	// I have not found a way on Linux to display the floating widget behind the
	// dock overlay. That means if the user drags this floating widget around,
	// it is always painted in front of the dock overlay and dock overlay cross.
	// and the user will not see the dock overlay. To work around this issue,
	// the window opacity is set to 0.6 to make the dock overlay visible
	// again. If someone has an idea, how to place the dragged floating widget
	// behind the dock overlay, then a pull request would be welcome.
	if (DraggingFloatingWidget == DragState)
	{
		setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
		setWindowOpacity(0.6);
		d->MouseEventHandler = MouseEventHandler;
		if (d->MouseEventHandler)
		{
			d->MouseEventHandler->grabMouse();
		}
	}
#endif
	moveFloating();
	show();

}

//============================================================================
void CFloatingDockContainer::moveFloating()
{
	int BorderSize = (frameSize().width() - size().width()) / 2;
	const QPoint moveToPos = QCursor::pos() - d->DragStartMousePosition
	    - QPoint(BorderSize, 0);
	move(moveToPos);
}

//============================================================================
bool CFloatingDockContainer::isClosable() const
{
	return d->DockContainer->features().testFlag(
	    CDockWidget::DockWidgetClosable);
}

//============================================================================
void CFloatingDockContainer::onDockAreasAddedOrRemoved()
{
	ADS_PRINT("CFloatingDockContainer::onDockAreasAddedOrRemoved()");
	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		d->SingleDockArea = TopLevelDockArea;
		d->setWindowTitle(
		    d->SingleDockArea->currentDockWidget()->windowTitle());
		connect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
		    SLOT(onDockAreaCurrentChanged(int)));
	}
	else
	{
		if (d->SingleDockArea)
		{
			disconnect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
			    SLOT(onDockAreaCurrentChanged(int)));
			d->SingleDockArea = nullptr;
		}
		d->setWindowTitle(qApp->applicationDisplayName());
	}
}

//============================================================================
void CFloatingDockContainer::updateWindowTitle()
{
	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		d->setWindowTitle(TopLevelDockArea->currentDockWidget()->windowTitle());
	}
	else
	{
		d->setWindowTitle(qApp->applicationDisplayName());
	}
}

//============================================================================
void CFloatingDockContainer::onDockAreaCurrentChanged(int Index)
{
	Q_UNUSED(Index);
	d->setWindowTitle(d->SingleDockArea->currentDockWidget()->windowTitle());
}

//============================================================================
bool CFloatingDockContainer::restoreState(QXmlStreamReader &Stream,
    bool Testing)
{
	if (!d->DockContainer->restoreState(Stream, Testing))
	{
		return false;
	}

	onDockAreasAddedOrRemoved();
	return true;
}

//============================================================================
bool CFloatingDockContainer::hasTopLevelDockWidget() const
{
	return d->DockContainer->hasTopLevelDockWidget();
}

//============================================================================
CDockWidget* CFloatingDockContainer::topLevelDockWidget() const
{
	return d->DockContainer->topLevelDockWidget();
}

//============================================================================
QList<CDockWidget*> CFloatingDockContainer::dockWidgets() const
{
	return d->DockContainer->dockWidgets();
}

//============================================================================
void CFloatingDockContainer::finishDragging()
{
	ADS_PRINT("CFloatingDockContainer::finishDragging");
#ifdef Q_OS_LINUX
   setAttribute(Qt::WA_X11NetWmWindowTypeDock, false);
   setWindowOpacity(1);
   activateWindow();
   if (d->MouseEventHandler)
   {
       d->MouseEventHandler->releaseMouse();
       d->MouseEventHandler = nullptr;
   }
#endif
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF FloatingDockContainer.cpp
