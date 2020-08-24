#pragma once

#include <QWidget>
#include <vector>

class QWidget;
struct HippoMessageAPI;
struct HippoServiceAPI;
class Ui_Console;
struct HippoLogMessages;
class QSyntaxHighlighter;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ConsoleView : public QWidget
{
    Q_OBJECT
public:

    ConsoleView(const struct HippoMessageAPI* messages_api, QWidget* parent);
    virtual ~ConsoleView();

    void incoming_messages(const unsigned char* data, int len);

private:
    Q_SLOT void checked(bool state);
    Q_SLOT void select_log_file(bool checked);
    Q_SLOT void clear_log();

    void log_messages(const struct HippoLogMessages* messages);
    void send_log_enable(bool state);
    void write_settings();
    void read_settings();

    const struct HippoMessageAPI* m_message_api = nullptr;
    Ui_Console* m_ui = nullptr;
    QSyntaxHighlighter* m_highlighter = nullptr;

    QString m_log_file;

    // used to check what kind mask the line has so we can easily hide/unhide fast when settings change
    uint8_t m_active_mask = 0xff;
    std::vector<uint8_t> m_line_masks;
};

