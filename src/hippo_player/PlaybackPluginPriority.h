#pragma once

#include <QDialog>
#include <QtWidgets/QListWidget>

class Ui_PlaybackPluginPriority;
class QListWidgetItem;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PluginList : public QListWidget {
    Q_OBJECT
public:
    PluginList(QWidget* parent);

protected:
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* event);
    void startDrag(Qt::DropActions supportedActions);
    void dragEnterEvent(QDragEnterEvent* event);
    Qt::DropAction supportedDropActions();
signals:
    void itemDroped();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PlaybackPluginPriority shows settings for font and interface, etc. Everything that
// has to do with colors and such
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlaybackPluginPriority : public QDialog {
    Q_OBJECT

public:
    explicit PlaybackPluginPriority(QWidget* parent = nullptr);
    virtual ~PlaybackPluginPriority();

private:
    Ui_PlaybackPluginPriority* m_ui = nullptr;
    PluginList* m_plugin_list = nullptr;
};
