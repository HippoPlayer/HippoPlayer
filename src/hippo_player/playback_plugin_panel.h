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

private:
    void build_ui(QGroupBox* group_box, const HSSetting* settings, int count);

    Q_SLOT void change_plugin(QTreeWidgetItem* curr, QTreeWidgetItem* prev);
    const struct HippoCore* m_core;
    Ui_PlaybackPluginPanel* m_ui = nullptr;
    QListWidget* m_plugin_list = nullptr;
    QVBoxLayout* m_settings_layout = nullptr;
    std::vector<QWidget*> m_widgets;
    std::vector<std::string> m_plugin_names;

    PluginSettings m_global_settings;
    std::vector<PluginSettings> m_file_type_settings;
};

