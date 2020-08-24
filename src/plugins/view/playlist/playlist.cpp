#include "playlist.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QStyleOptionViewItem>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

#include "../../../plugin_api/HippoMessages.h"
#include "../../../plugin_api/HippoPlugin.h"

#if 0

class Delegate : public QStyledItemDelegate {
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        if((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver))
        {
            // get the color to paint with
            QVariant var = index.model()->data(index, Qt::BackgroundRole);

            // draw the row and its content
            painter->fillRect(option.rect, var.value<QColor>());
            painter->drawText(option.rect, index.model()->data(index, Qt::DisplayRole).toString());
        }
        else

        QVariant var = index.model()->data(index, Qt::BackgroundRole);
        painter->fillRect(option.rect, var.value<QColor>());
        QStyledItemDelegate::paint(painter, option, index);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DeselectableTreeView : public QListView
{
public:
    DeselectableTreeView(QWidget *parent) : QListView(parent) {}
    virtual ~DeselectableTreeView() {}

private:
    virtual void mousePressEvent(QMouseEvent *event) {
        QModelIndex item = indexAt(event->pos());
        bool selected = selectionModel()->isSelected(item);
        printf("selected %d\n", selected);
        QListView::mousePressEvent(event);
        if (selected)
            selectionModel()->select(item, QItemSelectionModel::Deselect);
    }
};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TreeView : public QTreeView {
   public:
    TreeView(QWidget* parent) : QTreeView(parent) { _pixmapBg.load("data/hippo.png"); }

    void paintEvent(QPaintEvent* event) {
        QPainter painter(viewport());

        // Draw a hippo (like classic hippo player) if there are no entries in the playlist
        if (model()->rowCount() == 0) {
            painter.setRenderHint(QPainter::SmoothPixmapTransform);

            auto winSize = size();
            auto pixmapRatio = (float)_pixmapBg.width() / _pixmapBg.height();
            auto windowRatio = (float)winSize.width() / winSize.height();

            if (pixmapRatio > windowRatio) {
                auto newWidth = (int)(winSize.height() * pixmapRatio);
                auto offset = (newWidth - winSize.width()) / -2;
                painter.drawPixmap(offset, 0, newWidth, winSize.height(), _pixmapBg);
            } else {
                auto newHeight = (int)(winSize.width() / pixmapRatio);
                auto offset = (newHeight - winSize.height()) / -2;
                painter.drawPixmap(0, offset, winSize.width(), newHeight, _pixmapBg);
            }
        }

        return QTreeView::paintEvent(event);
    }

    /*
    const QModelIndexList& selection() {
        return selectedIndexes();
    }
    */

    QPixmap _pixmapBg;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* PlaylistView::create(struct HippoServiceAPI* service_api, QAbstractItemModel* model) {
    m_message_api = HippoServiceAPI_get_message_api(service_api, HIPPO_MESSAGE_API_VERSION);

    QWidget* widget = new QWidget;
    QVBoxLayout* vbox = new QVBoxLayout(widget);

    QPushButton* delete_items_button = new QPushButton(QStringLiteral("Delete items.."));

    m_list = new TreeView(nullptr);
    m_list->setModel(model);
    m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // m_list->setStyleSheet(QStringLiteral("QTreeView { border-image: url(data/hippo.png) no-repeat center center
    // fixed; background-origin: content; }"));

    // border-image: url(data/hippo.png) 0 0 0 0 stretch stretch; }"));
    // m_list->setItemDelegate(new Delegate());

    // QStandardItemModel* model1 = new QStandardItemModel(0, 3, nullptr);
    // model1->setHeaderData(0, Qt::Horizontal, "Title");
    // model1->setHeaderData(1, Qt::Horizontal, "Duration");
    // model1->setHeaderData(2, Qt::Horizontal, "Information");
    // m_list->setSelectionBehavior(QAbstractItemView::SelectRows);

    // m_list->header()->setModel(model1);
    vbox->addWidget(m_list);
    vbox->addWidget(delete_items_button);

    QObject::connect(m_list, &QTreeView::doubleClicked, this, &PlaylistView::item_double_clicked);
    // QShortcut* delete_items = new QShortcut(QKeySequence(Qt::Key_Delete), delete_items_button);
    // delete_items->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(delete_items_button, &QPushButton::pressed, this, &PlaylistView::delete_items);
    // QObject::connect(delete_items, &QShortcut::activated, this, &PlaylistView::delete_items);

    // Have the core send us the current list of songs
    flatbuffers::FlatBufferBuilder builder(1024);
    builder.Finish(CreateHippoMessageDirect(builder, MessageType_request_added_urls,
                                            CreateHippoRequestAddedUrls(builder).Union()));

    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());

    widget->setWindowTitle(QStringLiteral("Playlist"));

    return widget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::select_song(const HippoSelectSong* msg) {
    int playlist_index = msg->playlist_index();
    const QModelIndex item = m_list->model()->index(playlist_index, 0);
    m_list->setCurrentIndex(item);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::play_entry(const QModelIndex& item, bool pause_state) {
    QAbstractItemModel* model = m_list->model();

    flatbuffers::FlatBufferBuilder builder(1024);
    QVariant v = model->data(item, Qt::UserRole);
    QByteArray path = v.toByteArray();

    builder.Finish(CreateHippoMessageDirect(
        builder, MessageType_request_select_song,
        CreateHippoRequestSelectSongDirect(builder, pause_state, path.data(), model->buddy(item).row()).Union()));

    HippoMessageAPI_send(m_message_api, builder.GetBufferPointer(), builder.GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::item_double_clicked(const QModelIndex& item) {
    play_entry(item, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::delete_items() {
    QAbstractItemModel* model = m_list->model();
    auto selection_model = m_list->selectionModel();
    auto list = selection_model->selectedIndexes();

    if (list.size() >= 3) {
        // TODO: we should batch the removal of items if they are all in order
        // step 3 for I as we go by rows and not each item
        for (int i = 0, len = list.size(); i < len; i += 3) {
            QModelIndex index = list.at(i);
            model->removeRows(index.row(), 1);
        }
    }

    model->layoutChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlaylistView::incoming_messages(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    switch (message->message_type()) {
        case MessageType_select_song: {
            select_song(message->message_as_select_song());
            break;
        }

        case MessageType_request_play_song: {
            auto msg = message->message_as_request_play_song();
            // TODO: this will leak but for some reason we crash here otherwise :(
            QModelIndexList* indices = new QModelIndexList(m_list->selectionModel()->selectedIndexes());

            if (indices->count() > 0) {
                play_entry(indices->at(0), msg->pause_state());
            }
            break;
        }

        default:
            break;
    }
}
