#include "prefs_dialog.h"
#include "playback_plugin_priority.h"
#include "playback_plugin_panel.h"
#include "device_panel.h"
#include "ui_prefs_dialog.h"
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QPushButton>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Config {
    QString name;
    QWidget* widget;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PrefsDialog::PrefsDialog(const struct HippoCore* core, const struct HippoMessageAPI* messages_api, QWidget* parent)
    : QDialog(parent), m_ui(new Ui_PrefsDialog) {
    m_ui->setupUi(this);

    hide();

    m_plugin_panel = new PlaybackPluginPanel(core);
    m_plugin_priority = new PlaybackPluginPriority;
    m_device_panel = new DevicePanel(messages_api);

    m_ui->panel->addWidget(m_plugin_panel);
    m_ui->panel->addWidget(m_plugin_priority);
    m_ui->panel->addWidget(m_device_panel);

    QTreeWidgetItem* playback = new QTreeWidgetItem();
    playback->setExpanded(true);
    playback->setData(0, Qt::UserRole, -1);
    playback->setText(0, QStringLiteral("Playback"));

    QTreeWidgetItem* plugins = new QTreeWidgetItem();
    plugins->setData(0, Qt::UserRole, 0);
    plugins->setText(0, QStringLiteral("Plugins"));
    plugins->setExpanded(true);
    playback->addChild(plugins);

    QTreeWidgetItem* plugin_priority = new QTreeWidgetItem();
    plugin_priority->setData(0, Qt::UserRole, 1);
    plugin_priority->setText(0, QStringLiteral("Priority"));
    plugin_priority->setExpanded(true);
    playback->addChild(plugin_priority);

    QTreeWidgetItem* device_panel = new QTreeWidgetItem();
    device_panel->setExpanded(true);
    device_panel->setData(0, Qt::UserRole, 2);
    device_panel->setText(0, QStringLiteral("Output"));
    playback->addChild(device_panel);

    m_ui->config_category->addTopLevelItem(playback);
    m_ui->config_category->expandItem(playback);

    //m_ui->panel->setCurrentIndex(0);

    QObject::connect(m_ui->config_category, &QTreeWidget::currentItemChanged, this, &PrefsDialog::change_layout);
    QObject::connect(this, &QDialog::finished, this, &PrefsDialog::dialog_finished);
    QObject::connect(m_ui->button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(m_ui->button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(m_ui->button_box->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &PrefsDialog::reset);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrefsDialog::reset() {
    // this is a temp hack

    if (m_index == 0) {
        m_plugin_panel->reset();
    } else {
        //..
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrefsDialog::dialog_finished(int state) {
    if (state == 0) {
        if (m_index == 0) {
            m_plugin_panel->cancel();
        }
        m_device_panel->cancel();
    } else {
        if (m_index == 0) {
            m_plugin_panel->ok();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrefsDialog::change_layout(QTreeWidgetItem* curr, QTreeWidgetItem* prev) {
    if (!curr) curr = prev;

    m_index = curr->data(0, Qt::UserRole).toInt();

    if (m_index >= 0) {
        m_ui->panel->show();
        m_ui->panel->setCurrentIndex(m_index);
    } else {
        m_ui->panel->hide();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrefsDialog::incoming_messages(const unsigned char* data, int len) {
    m_device_panel->incoming_messages(data, len);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PrefsDialog::~PrefsDialog() { delete m_ui; }
