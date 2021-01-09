#pragma once

#include <QDialog>
#include <vector>
#include <string>
#include "src/hippo_core/native/hippo_core.h"

class Ui_PlaybackPluginPanel;
class QListWidgetItem;
class QListWidget;
struct HippoCore;
class QTreeWidgetItem;
class QVBoxLayout;
class QWidget;
class QGroupBox;
union HSSetting;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PlaybackPluginPanel is used to re-order playback plugins in their priority order
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlaybackPluginPanel : public QDialog {
    Q_OBJECT

public:
    explicit PlaybackPluginPanel(const struct HippoCore* core, QWidget* parent = nullptr);
    virtual ~PlaybackPluginPanel();

    void reset();
    void cancel();
    void ok();

private:
    void build_ui(QVBoxLayout* group_layout, const HSSetting* settings, int count, int pixel_width, int id_offset);

    Q_SLOT void change_plugin(QTreeWidgetItem* curr, QTreeWidgetItem* prev);
    Q_SLOT void change_int(int v);
    Q_SLOT void change_fixed_int(int v);
    Q_SLOT void change_fixed_string(int v);
    Q_SLOT void change_bool(int v);
    Q_SLOT void change_double(double v);

    void update_widgets();

    union HSSetting* get_setting_from_id(QObject* sender);

    const struct HippoCore* m_core;
    Ui_PlaybackPluginPanel* m_ui = nullptr;
    QListWidget* m_plugin_list = nullptr;
    QVBoxLayout* m_layout = nullptr;

    std::vector<QWidget*> m_widgets;
    std::vector<std::string> m_plugin_names;

    // used for when pressing cancel
    union HSSetting* m_old_settings = nullptr;

    PluginSettings m_settings;
    std::vector<int> m_widget_indices;

    int m_active_file_ext_index = 0;
    const char* m_active_plugin_name = nullptr;
};

