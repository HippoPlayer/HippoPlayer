#ifndef DockWidgetTabH
#define DockWidgetTabH
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
/// \file   DockWidgetTab.h
/// \author Uwe Kindler
/// \date   27.02.2017
/// \brief  Declaration of CDockWidgetTab class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"

namespace ads
{
class CDockWidget;
class CDockAreaWidget;
struct DockWidgetTabPrivate;

/**
 * A dock widget tab that shows a title and an icon.
 * The dock widget tab is shown in the dock area title bar to switch between
 * tabbed dock widgets
 */
class ADS_EXPORT CDockWidgetTab : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(bool activeTab READ isActiveTab WRITE setActiveTab NOTIFY activeTabChanged)

private:
	DockWidgetTabPrivate* d; ///< private data (pimpl)
	friend struct DockWidgetTabPrivate;

private slots:
	void onDetachActionTriggered();

protected:
	virtual void mousePressEvent(QMouseEvent* ev) override;
	virtual void mouseReleaseEvent(QMouseEvent* ev) override;
	virtual void mouseMoveEvent(QMouseEvent* ev) override;
	virtual void contextMenuEvent(QContextMenuEvent* ev) override;

	/**
	 * Double clicking the tab widget makes the assigned dock widget floating
	 */
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

public:
	using Super = QFrame;
	/**
	 * Default Constructor
	 * param[in] DockWidget The dock widget this title bar belongs to
	 * param[in] parent The parent widget of this title bar
	 */
	CDockWidgetTab(CDockWidget* DockWidget, QWidget* parent = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockWidgetTab();

	/**
	 * Returns true, if this is the active tab
	 */
	bool isActiveTab() const;

	/**
	 * Set this true to make this tab the active tab
	 */
	void setActiveTab(bool active);

	/**
	 * Returns the dock widget this title widget belongs to
	 */
	CDockWidget* dockWidget() const;

	/**
	 * Sets the dock area widget the dockWidget returned by dockWidget()
	 * function belongs to.
	 */
	void setDockAreaWidget(CDockAreaWidget* DockArea);

	/**
	 * Returns the dock area widget this title bar belongs to.
	 * \return This function returns 0 if the dock widget that owns this title
	 * bar widget has not been added to any dock area yet.
	 */
	CDockAreaWidget* dockAreaWidget() const;

	/**
	 * Sets the icon to show in title bar
	 */
	void setIcon(const QIcon& Icon);

	/**
	 * Returns the icon
	 */
	const QIcon& icon() const;

	/**
	 * Returns the tab text
	 */
	QString text() const;


	/**
	 * Sets the tab text
	 */
	void setText(const QString& title);

	/**
	 * This function returns true if the assigned dock widget is closeable
	 */
	bool isClosable() const;


	/**
	* Track event ToolTipChange and set child ToolTip 
	*/
	virtual bool event(QEvent *e) override;

public slots:
	  virtual void setVisible(bool visible) override;


signals:
	void activeTabChanged();
	void clicked();
	void closeRequested();
	void closeOtherTabsRequested();
	void moved(const QPoint& GlobalPos);
}; // class DockWidgetTab
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockWidgetTabH
