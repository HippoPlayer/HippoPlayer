#include "playback_plugin_priority.h"
#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QDragMoveEvent>
#include <QtWidgets/QListWidget>
#include "ui_playback_plugin_priority.h"
//#include <QtWidgets/QDragMoveEvent>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlaybackPluginPriority::PlaybackPluginPriority(QWidget* parent) : QDialog(parent), m_ui(new Ui_PlaybackPluginPriority) {
    m_ui->setupUi(this);

    QListWidget* list = new QListWidget();

    for (int i = 1; i < 10; ++i) {
        auto item = new QListWidgetItem(QString(QStringLiteral("Plugin %1")).arg(i));
        item->setCheckState(Qt::Checked);
        item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
        list->addItem(item);
    }

    list->setDragDropMode(QAbstractItemView::InternalMove);

    /*
    lw.show();

    auto list = new PluginList(this);

    auto t0 = new QListWidgetItem(QStringLiteral("Test 0"));
    auto t1 = new QListWidgetItem(QStringLiteral("Test 1"));

    list->addItem(t0);
    list->addItem(t1);
    */

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(list);
    // vbox->addStretch(1);
    m_ui->groupbox->setLayout(vbox);

    /*

    auto config = Config::instance();
    auto themes = config->theme_list();

    m_ui->theme_combo->clear();

    for (auto& theme : themes) {
        m_ui->theme_combo->addItem(theme);
    }

    m_ui->theme_combo->setCurrentIndex(Config::instance()->current_theme());
    m_ui->theme_combo->setMaxVisibleItems(10);

    connect(m_ui->theme_combo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), config,
            &Config::load_theme);
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlaybackPluginPriority::~PlaybackPluginPriority() {
    delete m_ui;
}
