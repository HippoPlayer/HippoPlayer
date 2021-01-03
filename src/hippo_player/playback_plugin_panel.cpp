#include "playback_plugin_panel.h"
#include <QtCore/QDebug>
#include <QtGui/QDrag>
#include <QtGui/QDragMoveEvent>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include "src/hippo_core/native/hippo_core.h"
#include "src/plugin_api/HippoSettings.h"
#include "ui_playback_plugin_panel.h"

//#include <QtWidgets/QDragMoveEvent>

const int ID_OFFSET = 1000;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlaybackPluginPanel::PlaybackPluginPanel(const struct HippoCore* core, QWidget* parent)
    : QDialog(parent), m_core(core), m_ui(new Ui_PlaybackPluginPanel) {
    m_ui->setupUi(this);

    QStringList headerLabels;
    headerLabels.push_back(QStringLiteral("Name"));
    headerLabels.push_back(QStringLiteral("Version"));
    headerLabels.push_back(QStringLiteral("Library Version"));
    m_ui->plugin_list->setColumnCount(headerLabels.count());
    m_ui->plugin_list->setHeaderLabels(headerLabels);

    for (int i = 0;; ++i) {
        PluginInfo info = hippo_get_playback_plugin_info((HippoCore*)m_core, i);

        if (!info.name) {
            break;
        }

        QString name = QString::fromUtf8(info.name, info.name_len);
        QString version = QString::fromUtf8(info.version, info.version_len);
        QString lib_name = QString::fromUtf8(info.library, info.library_len);

        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setData(0, Qt::UserRole, i);
        item->setText(0, name);
        item->setText(1, version);
        item->setText(2, lib_name);

        m_plugin_names.push_back(std::string(info.name, info.name_len));
        m_ui->plugin_list->addTopLevelItem(item);
    }

    QObject::connect(m_ui->plugin_list, &QTreeWidget::currentItemChanged, this, &PlaybackPluginPanel::change_plugin);

    // m_ui->settings_group->setLayout(m_settings_layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int calculate_pixel_width(const HSSetting* setting, int count) {
    // first calculate the longest text so we can align the ui

    const char* long_name = nullptr;
    size_t max_len = 0;

    for (int i = 0; i < count; ++i) {
        HSBase* base = (HSBase*)&setting[i];
        size_t len = strlen(base->name);

        if (len > max_len) {
            long_name = base->name;
            max_len = len;
        }
    }

    QLabel dummy;

    return (int)dummy.fontMetrics().boundingRect(QString::fromUtf8(long_name)).width();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaybackPluginPanel::change_plugin(QTreeWidgetItem* curr, QTreeWidgetItem* prev) {
    QLayoutItem* child = nullptr;

    if (!curr)
        curr = prev;

    // remove the old widgets first

    if (m_global_layout) {
        while ((child = m_global_layout->takeAt(0)) != 0) {
            delete child;
        }
    }

    if (m_file_ext_layout) {
        while ((child = m_file_ext_layout->takeAt(0)) != 0) {
            delete child;
        }
    }

    for (auto& t : m_widgets) {
        delete t;
    }

    m_widgets.clear();

    // clear the old settings
    m_file_type_settings.clear();
    m_widget_indices.clear();

    m_global_layout = new QVBoxLayout;
    m_file_ext_layout = new QVBoxLayout;

    int index = curr->data(0, Qt::UserRole).toInt();
    const char* name = m_plugin_names[index].c_str();

    // fetch the settings.
    m_global_settings = hippo_get_playback_plugin_settings((HippoCore*)m_core, name, 0);

    for (int i = 0;; ++i) {
        PluginSettings info = hippo_get_playback_plugin_settings((HippoCore*)m_core, name, i + 1);

        if (!info.settings_count) {
            break;
        }

        std::string name = std::string(info.name, info.name_len);

        printf("settings ptr %p\n", info.settings);

        if (info.settings_count > 0) {
            m_file_type_settings.push_back(info);
        }
    }

    if (m_global_settings.settings_count > 0) {
        QGroupBox* group_box = new QGroupBox(QStringLiteral("Global"));
        int pixel_width = calculate_pixel_width(m_global_settings.settings, m_global_settings.settings_count);
        build_ui(m_global_layout, m_global_settings.settings, m_global_settings.settings_count, pixel_width, 0);
        m_ui->layout->addWidget(group_box);
        group_box->setLayout(m_global_layout);
        m_widgets.push_back(group_box);
    }

    if (m_file_type_settings.size() > 0) {
        QGroupBox* group_box = new QGroupBox(QStringLiteral("File extension"));
        int pixel_width =
            calculate_pixel_width(m_file_type_settings[0].settings, m_file_type_settings[0].settings_count);
        add_file_type_selection(pixel_width);
        build_ui(m_file_ext_layout, m_file_type_settings[0].settings, m_file_type_settings[0].settings_count,
                 pixel_width, ID_OFFSET);
        m_ui->layout->addWidget(group_box);
        group_box->setLayout(m_file_ext_layout);
        m_widgets.push_back(group_box);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaybackPluginPanel::add_file_type_selection(int pixel_width) {
    QLabel* label_text = new QLabel(QStringLiteral("File extension"));
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(label_text, 0, 0);
    layout->setColumnMinimumWidth(0, pixel_width);
    layout->setColumnStretch(1, 0);
    QComboBox* combo_box = new QComboBox;
    combo_box->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    combo_box->setToolTip(QStringLiteral(
        "Change settings on global/file extension basis. Changes on file type will override the global values"));

    for (int i = 0, size = m_file_type_settings.size(); i < size; ++i) {
        combo_box->addItem(QString::fromUtf8(m_file_type_settings[i].name, m_file_type_settings[i].name_len));
    }

    QObject::connect(combo_box, QOverload<int>::of(&QComboBox::activated), this, &PlaybackPluginPanel::change_file_ext);

    layout->addWidget(combo_box, 0, 1);
    m_file_ext_layout->addLayout(layout);

    m_widgets.push_back(label_text);
    m_widgets.push_back(combo_box);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HSSetting* PlaybackPluginPanel::get_setting_from_id(QObject* sender) {
    QVariant data = sender->property("hippo_data");
    const int id = data.toInt();

    // check if we are changing file ext settings or global
    if (id >= ID_OFFSET) {
        printf("settings index file ext %d - index %d\n", m_active_file_ext_index, id - ID_OFFSET);
        return &m_file_type_settings[m_active_file_ext_index].settings[id - ID_OFFSET];
    } else {
        return &m_global_settings.settings[id];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaybackPluginPanel::change_int(int v) {
    HSSetting* setting = get_setting_from_id(sender());
    setting->int_value.value = v;

    QVariant data = sender()->property("hippo_data");
    printf("id %d - val %d \n", data.toInt(), v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaybackPluginPanel::change_bool(int v) {
    HSSetting* setting = get_setting_from_id(sender());
    setting->bool_value.value = v == Qt::Checked ? true : false;

    QVariant data = sender()->property("hippo_data");
    printf("id %d - bool %d\n", data.toInt(), v == Qt::Checked ? 1 : 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaybackPluginPanel::change_double(double v) {
    HSSetting* setting = get_setting_from_id(sender());
    setting->float_value.value = v;

    QVariant data = sender()->property("hippo_data");
    printf("id %d - val %f - %p\n", data.toInt(), v, setting);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// When we change file ext we need to update all the existing widgets and the active index

Q_SLOT void PlaybackPluginPanel::change_file_ext(int index) {
    m_active_file_ext_index = index;

    printf("new index %d index\n", index);

    HSSetting* settings = m_file_type_settings[index].settings;
    int count = m_file_type_settings[index].settings_count;

    for (int i = 0; i < count; ++i) {
        HSBase* base = (HSBase*)&settings[i];
        int widget_index = m_widget_indices[i];

        switch (base->widget_type) {
            case HS_FLOAT_TYPE: {
                printf("updating id %d with value %f %p\n", i,
                    settings[i].float_value.value,
                    &settings[i]);

                QDoubleSpinBox* v = (QDoubleSpinBox*)m_widgets[widget_index];
                v->setValue(settings[i].float_value.value);
                break;
            }

            case HS_INTEGER_TYPE: {
                QSpinBox* v = (QSpinBox*)m_widgets[widget_index];
                v->setValue(settings[i].int_value.value);
                break;
            }

            case HS_INTEGER_RANGE_TYPE: {
                printf("fetching combo box at %d\n", widget_index);
                QComboBox* v = (QComboBox*)m_widgets[widget_index];
                v->setCurrentIndex(settings[i].int_value.value);
                break;
            }

            case HS_BOOL_TYPE: {
                QCheckBox* v = (QCheckBox*)m_widgets[widget_index];
                v->setCheckState(settings[i].bool_value.value ? Qt::Checked : Qt::Unchecked);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaybackPluginPanel::build_ui(QVBoxLayout* group_layout, const HSSetting* setting, int count, int pixel_width,
                                   int id_offset) {
    // combo box with all types

    // pixel_width = 300;

    for (int i = 0; i < count; ++i) {
        HSBase* base = (HSBase*)setting;

        QLabel* label_text = new QLabel(QString::fromUtf8(base->name));
        QGridLayout* layout = new QGridLayout;
        layout->addWidget(label_text, i, 0);
        layout->setColumnMinimumWidth(0, pixel_width);
        layout->setColumnStretch(1, 0);

        QString tool_tip = QString::fromUtf8(base->desc);

        int widget_id = i + id_offset;

        switch (base->widget_type) {
            case HS_FLOAT_TYPE: {
                QDoubleSpinBox* spin_box = new QDoubleSpinBox;
                spin_box->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

                spin_box->setProperty("hippo_data", QVariant(widget_id));
                QObject::connect(spin_box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
                                 &PlaybackPluginPanel::change_double);

                if (id_offset == ID_OFFSET) {
                    m_widget_indices.push_back(int(m_widgets.size()));
                }

                m_widgets.push_back(spin_box);

                if (setting->float_value.start_range != setting->float_value.end_range) {
                    spin_box->setRange(setting->float_value.start_range, setting->float_value.end_range);
                    spin_box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

                    layout->setColumnStretch(1, 0);

                    // if we have a range add a slider also

                    QSlider* slider = new QSlider(Qt::Horizontal);
                    slider->setToolTip(tool_tip);
                    slider->setMinimum(int(setting->float_value.start_range * 100));
                    slider->setMaximum(int(setting->float_value.end_range * 100));
                    layout->addWidget(slider, i, 2);
                    m_widgets.push_back(slider);

                    QObject::connect(spin_box, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                     [slider](double i) { slider->setValue(int(i * 100)); });
                    QObject::connect(slider, QOverload<int>::of(&QSlider::valueChanged),
                                     [spin_box](int v) -> void { spin_box->setValue(v / 100.0f); });
                } else {
                    spin_box->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
                }

                spin_box->setToolTip(tool_tip);
                spin_box->setSingleStep(0.1);
                spin_box->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
                spin_box->setValue(setting->float_value.value);

                layout->addWidget(spin_box, i, 1);

                break;
            }

            case HS_INTEGER_TYPE: {
                QSpinBox* spin_box = new QSpinBox;

                spin_box->setToolTip(tool_tip);
                spin_box->setSingleStep(1);
                spin_box->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);

                spin_box->setProperty("hippo_data", QVariant(widget_id));
                QObject::connect(spin_box, QOverload<int>::of(&QSpinBox::valueChanged), this,
                                 &PlaybackPluginPanel::change_int);

                if (id_offset == ID_OFFSET) {
                    m_widget_indices.push_back(int(m_widgets.size()));
                }

                m_widgets.push_back(spin_box);

                layout->addWidget(spin_box, i, 1);

                if (setting->int_value.start_range != setting->int_value.end_range) {
                    spin_box->setRange(setting->int_value.start_range, setting->int_value.end_range);
                    spin_box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

                    layout->setColumnStretch(1, 0);

                    // if we have a range add a slider also

                    QSlider* slider = new QSlider(Qt::Horizontal);
                    slider->setToolTip(tool_tip);
                    slider->setMinimum(setting->int_value.start_range);
                    slider->setMaximum(setting->int_value.end_range);
                    layout->addWidget(slider, i, 2);
                    m_widgets.push_back(slider);

                    QObject::connect(spin_box, QOverload<int>::of(&QSpinBox::valueChanged),
                                     [slider](int i) { slider->setValue(i); });
                    QObject::connect(slider, QOverload<int>::of(&QSlider::valueChanged),
                                     [spin_box](int v) -> void { spin_box->setValue(v); });
                } else {
                    spin_box->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
                }

                spin_box->setValue(setting->int_value.value);
                printf("start value %d\n", setting->int_value.value);

                break;
            }

            case HS_INTEGER_RANGE_TYPE: {
                QComboBox* combo_box = new QComboBox;
                combo_box->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
                const HSIntegerFixedRange* range = &setting->int_fixed_value;

                if (id_offset == ID_OFFSET) {
                    printf("adding combo box at %d\n", int(m_widgets.size()));
                    m_widget_indices.push_back(int(m_widgets.size()));
                }

                m_widgets.push_back(combo_box);

                combo_box->setProperty("hippo_data", QVariant(widget_id));
                QObject::connect(combo_box, QOverload<int>::of(&QComboBox::activated), this,
                                 &PlaybackPluginPanel::change_int);

                for (int p = 0; p < range->values_count; ++p) {
                    combo_box->addItem(QString::fromUtf8(range->values[p].name), QVariant(range->values[p].value));
                }

                combo_box->setToolTip(tool_tip);
                layout->addWidget(combo_box, i, 1);
                break;
            }

            case HS_STRING_RANGE_TYPE: {
                QComboBox* combo_box = new QComboBox;
                const HSStringFixedRange* range = &setting->string_fixed_value;
                combo_box->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

                combo_box->setProperty("hippo_data", QVariant(widget_id));
                QObject::connect(combo_box, QOverload<int>::of(&QComboBox::activated), this,
                                 &PlaybackPluginPanel::change_int);

                for (int p = 0; p < range->values_count; ++p) {
                    combo_box->addItem(QString::fromUtf8(range->values[p].name),
                                       QVariant(QString::fromUtf8(range->values[p].value)));
                }

                printf("adding combo box at %d\n", int(m_widgets.size()));

                if (id_offset == ID_OFFSET) {
                    printf("adding combo box at %d\n", int(m_widgets.size()));
                    m_widget_indices.push_back(int(m_widgets.size()));
                }

                m_widgets.push_back(combo_box);

                combo_box->setToolTip(tool_tip);
                layout->addWidget(combo_box, i, 1);
                break;
            }

            case HS_BOOL_TYPE: {
                QCheckBox* check_box = new QCheckBox;
                const HSBool* b_value = &setting->bool_value;

                check_box->setProperty("hippo_data", QVariant(widget_id));
                QObject::connect(check_box, QOverload<int>::of(&QCheckBox::stateChanged), this,
                                 &PlaybackPluginPanel::change_bool);

                if (id_offset == ID_OFFSET) {
                    m_widget_indices.push_back(int(m_widgets.size()));
                }

                m_widgets.push_back(check_box);

                check_box->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
                check_box->setCheckState(b_value->value ? Qt::Checked : Qt::Unchecked);
                check_box->setToolTip(tool_tip);
                layout->addWidget(check_box, i, 1);
                break;
            }

            default:
                break;
        }

        setting++;
        // m_settings_layout->addLayout(layout);
        group_layout->addLayout(layout);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlaybackPluginPanel::~PlaybackPluginPanel() {
    delete m_ui;
}
