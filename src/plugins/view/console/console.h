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
struct HippoLogMessages;
class QSyntaxHighlighter;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ConsoleView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "console_view.json")
    Q_INTERFACES(HippoQtView)
    virtual ~ConsoleView();

public:
    QWidget* create(struct HippoServiceAPI* service_api, QAbstractItemModel* model);
    void event(const unsigned char* data, int len);
    void log_messages(const struct HippoLogMessages* messages);

private:
    const struct HippoMessageAPI* m_message_api = nullptr;
    QFont* m_text_font = nullptr;
    Ui_Console* m_ui;
    QWidget* m_base;
    QSyntaxHighlighter* m_highlighter = nullptr;
};

