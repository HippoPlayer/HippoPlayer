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


    /*
    Config configs[] = {
        {QStringLiteral("Apperance"), new AppearanceWidget(this)},
    };

    QListWidgetItem* first_item = nullptr;

    for (unsigned i = 0; i < sizeof_array(configs); ++i) {
        QListWidgetItem* item = new QListWidgetItem(configs[i].name);

        if (i == 0) {
            first_item = item;
        }

        QWidget* widget = configs[i].widget;

        item->setData(Qt::UserRole, i);

        m_ui->panel->addWidget(widget);
        m_ui->config_categories->addItem(item);
    }

    m_ui->config_categories->setCurrentItem(first_item);

    connect(m_ui->save_buttons, &QDialogButtonBox::accepted, this,
            &QDialog::close);
    */
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
