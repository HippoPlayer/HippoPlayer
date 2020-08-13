#pragma once

#include "../../../plugin_api/HippoQtView.h"
#include <vector>

class QWidget;
class QFont;
struct HippoMessageAPI;
struct HippoServiceAPI;
class QTreeView;
class QModelIndex;
class QVBoxLayout;
class QPlainTextEdit;
class Ui_Console;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ConsoleView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "console_view.json")
    Q_INTERFACES(HippoQtView)

public:
    QWidget* create(struct HippoServiceAPI* service_api, QAbstractItemModel* model);
    void event(const unsigned char* data, int len);

private:
    Ui_Console* m_ui;
    QWidget* m_base;
};

