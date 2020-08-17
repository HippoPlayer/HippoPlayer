#include "PlaybackPluginPriority.h"
#include "ui_PlaybackPluginPriority.h"
#include <QtGui/QDragMoveEvent>
#include <QtCore/QMimeData>
#include <QtCore/QDebug>
#include <QtGui/QDrag>
//#include <QtWidgets/QDragMoveEvent>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PluginList::dragMoveEvent(QDragMoveEvent* e) {
    if (e->mimeData()->hasFormat(QStringLiteral("application/x-item")) && e->source() != this) {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    } else
        e->ignore();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginList::PluginList(QWidget* parent) : QListWidget(parent) {
    this->setViewMode(QListView::IconMode);
    this->setIconSize(QSize(55, 55));
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragEnabled(true);
    this->setDefaultDropAction(Qt::MoveAction);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PluginList::dropEvent(QDropEvent* event) {
    //if (event->mimeData()->hasFormat(QStringLiteral("application/x-item"))) {
        event->accept();
        event->setDropAction(Qt::MoveAction);
        QListWidgetItem* item = new QListWidgetItem;
        //QString name = event->mimeData()->data(QStringLiteral("application/x-item"));
        item->setText(QStringLiteral("dummy"));
        item->setIcon(QIcon(QStringLiteral(":/images/iString")));  // set path to image
        addItem(item);
    //}
    //else
    //   event->ignore();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PluginList::startDrag(Qt::DropActions supportedActions) {
    QListWidgetItem* item = currentItem();
    QMimeData* mimeData = new QMimeData;
    QByteArray ba;
    ba = item->text().toLatin1().data();
    //mimeData->setData(QStringLiteral("application/x-item"), ba);
    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
        delete takeItem(row(item));
        emit itemDroped();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PluginList::dragEnterEvent(QDragEnterEvent* event) {
    /*
    auto t = event->mimeData()->hasText();
    qDebug() << t;

    if (event->mimeData()->hasFormat(QStringLiteral("application/x-item")))
    */
        event->accept();
    /*
    else
        event->ignore();
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Qt::DropAction PluginList::supportedDropActions() {
    return Qt::MoveAction;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlaybackPluginPriority::PlaybackPluginPriority(QWidget* parent) : QDialog(parent), m_ui(new Ui_PlaybackPluginPriority) {
    m_ui->setupUi(this);

    QListWidget* list = new QListWidget();

    for(int i = 1; i < 10; ++i)
        list->addItem(new QListWidgetItem(QString(QStringLiteral("Item %1")).arg(i)));

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
    //vbox->addStretch(1);
    m_ui->groupBox->setLayout(vbox);

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
