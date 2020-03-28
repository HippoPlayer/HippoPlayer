#pragma once

#include "../../../plugin_api/HippoQtView.h"
#include <QtGui/QKeyEvent>

class QWidget;
struct HippoMessageAPI;
struct HippoServiceAPI;
struct HippoSelectSong;
class QTreeView;
class QModelIndex;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PlaylistView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "playlist_view.json")
    Q_INTERFACES(HippoQtView)

public:
    QWidget* create(struct HippoServiceAPI* service_api, QAbstractItemModel* model);
    void event(const unsigned char* data, int len);
    void delete_items();

private:
	void select_song(const HippoSelectSong* msg);

    const struct HippoMessageAPI* m_message_api = nullptr;
    QTreeView* m_list = nullptr;

    Q_SLOT void item_double_clicked(const QModelIndex& item);
};
