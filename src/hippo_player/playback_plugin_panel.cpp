#include "playback_plugin_panel.h"
#include <QtCore/QDebug>
#include <QtGui/QDrag>
#include <QtGui/QDragMoveEvent>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include "src/hippo_core/native/hippo_core.h"
#include "src/plugin_api/HippoSettings.h"
#include "ui_playback_plugin_panel.h"

//#include <QtWidgets/QDragMoveEvent>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlaybackPluginPanel::PlaybackPluginPanel(const struct HippoCore* core, QWidget* parent)
    : QDialog(parent), m_core(core), m_ui(new Ui_PlaybackPluginPanel) {
    m_ui->setupUi(this);

    m_settings_layout = new QVBoxLayout;

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

    m_ui->settings_group->setLayout(m_settings_layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaybackPluginPanel::change_plugin(QTreeWidgetItem* curr, QTreeWidgetItem* prev) {
    QLayoutItem* child = nullptr;

    if (!curr)
        curr = prev;

    // remove the old widgets first

    while ((child = m_settings_layout->takeAt(0)) != 0) {
        delete child;
    }

    for (auto& t : m_widgets) {
        delete t;
    }

    m_widgets.clear();

    // clear the old settings
    m_file_type_settings.clear();

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

        if (info.settings_count > 0) {
            m_file_type_settings.push_back(info);
        }
    }

    if (m_global_settings.settings_count > 0) {
        build_ui(m_global_settings.settings, m_global_settings.settings_count);
    }

    if (m_file_type_settings.size() > 0) {
        build_ui(m_file_type_settings[0].settings, m_file_type_settings[0].settings_count);
    }

    /*
    if (index == 0) {
        QLabel* label = new QLabel(QStringLiteral("Test"));

        m_settings_layout->addWidget(label);
        m_widgets.push_back(label);
    }
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaybackPluginPanel::build_ui(const HSSetting* setting, int count) {
    for (int i = 0; i < count; ++i) {
        HSBase* base = (HSBase*)setting;

        QHBoxLayout* layout = new QHBoxLayout;
        QLabel* label_text = new QLabel(QString::fromUtf8(base->name));

        layout->addWidget(label_text);

        switch (base->widget_type) {
            case HS_FLOAT_TYPE: {
                QDoubleSpinBox* spin_box = new QDoubleSpinBox;

                spin_box->setValue(setting->float_value.start_value);

                if (setting->float_value.start_range != setting->float_value.end_range) {
                    spin_box->setRange(setting->float_value.start_range, setting->float_value.end_range);
                }

                spin_box->setToolTip(QString::fromUtf8(base->desc));
                spin_box->setSingleStep(0.1);
                spin_box->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);

                layout->addWidget(spin_box);

                break;
            }

            case HS_INTEGER_TYPE: {
                QSpinBox* spin_box = new QSpinBox;

                spin_box->setValue(setting->int_value.start_value);

                if (setting->int_value.start_range != setting->int_value.end_range) {
                    spin_box->setRange(setting->int_value.start_range, setting->int_value.end_range);
                }

                spin_box->setToolTip(QString::fromUtf8(base->desc));
                spin_box->setSingleStep(1);
                spin_box->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);

                layout->addWidget(spin_box);
                break;
            }

            case HS_INTEGER_RANGE_TYPE: {
                QComboBox* combo_box = new QComboBox;
                const HSIntegerFixedRange* range = &setting->int_fixed_value;

                for (int i = 0; i < range->values_count; ++i) {
                    combo_box->addItem(QString::fromUtf8(range->values[i].name), QVariant(range->values[i].value));
                }

                combo_box->setToolTip(QString::fromUtf8(base->desc));
                layout->addWidget(combo_box);
                break;
            }

            case HS_STRING_RANGE_TYPE: {
                QComboBox* combo_box = new QComboBox;
                const HSStringFixedRange* range = &setting->string_fixed_value;

                for (int i = 0; i < range->values_count; ++i) {
                    combo_box->addItem(QString::fromUtf8(range->values[i].name),
                                       QVariant(QString::fromUtf8(range->values[i].value)));
                }

                combo_box->setToolTip(QString::fromUtf8(base->desc));
                layout->addWidget(combo_box);
                break;
            }

            case HS_BOOL_TYPE: {
                QCheckBox* check_box = new QCheckBox;
                const HSBool* b_value = &setting->bool_value;
                check_box->setCheckState(b_value->value ? Qt::Checked : Qt::Unchecked);
                check_box->setToolTip(QString::fromUtf8(base->desc));
                layout->addWidget(check_box);
                break;
            }

            default:
                break;
        }

        setting++;
        m_settings_layout->addLayout(layout);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlaybackPluginPanel::~PlaybackPluginPanel() {
    delete m_ui;
}
