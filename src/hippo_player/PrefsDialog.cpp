#include "PrefsDialog.h"
#include "PlaybackPluginPriority.h"
#include "ui_PrefsDialog.h"
#include <QtWidgets/QTreeWidgetItem>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Config {
    QString name;
    QWidget* widget;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PrefsDialog::PrefsDialog(QWidget* parent)
    : QDialog(parent), m_ui(new Ui_PrefsDialog) {
    m_ui->setupUi(this);

    printf("setting up ui\n");
    //setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));

    m_ui->panel->addWidget(new PlaybackPluginPriority);

    QTreeWidgetItem* playback = new QTreeWidgetItem();
    playback->setText(0, QStringLiteral("Playback"));
    QTreeWidgetItem* plugin_priority = new QTreeWidgetItem();
    plugin_priority->setText(0, QStringLiteral("Plugins"));

    playback->addChild(plugin_priority);

    m_ui->config_category->addTopLevelItem(playback);
    m_ui->panel->setCurrentIndex(0);

    QObject::connect(this, &QDialog::finished, this, &PrefsDialog::dialog_finished);
    QObject::connect(m_ui->button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(m_ui->button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrefsDialog::dialog_finished(int state) {
    printf("state %d\n", state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void PrefsDialog::change_layout(QListWidgetItem* curr, QListWidgetItem* prev) {
    if (!curr) curr = prev;

    int index = curr->data(Qt::UserRole).toInt();

    if (index) {
        m_ui->panel->setCurrentIndex(index - 1);
    }
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PrefsDialog::~PrefsDialog() { delete m_ui; }
