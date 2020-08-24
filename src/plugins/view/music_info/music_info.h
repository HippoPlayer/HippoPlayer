#pragma once

#include "../../../plugin_api/HippoQtView.h"
#include <vector>

class QWidget;
class QFont;
struct HippoMessageAPI;
struct HippoServiceAPI;
struct HippoSelectSong;
class QTreeView;
class QModelIndex;
class QVBoxLayout;
class QPlainTextEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MusicInfoView : public QObject, HippoQtView
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HippoQtView_iid FILE "music_info_view.json")
    Q_INTERFACES(HippoQtView)

public:
    QWidget* create(struct HippoServiceAPI* service_api, QAbstractItemModel* model);
    void incoming_messages(const unsigned char* data, int len);
    void delete_items();

private:
	void select_song(const HippoSelectSong* msg);
    void create_label_text(const QString& label_name, const QString& text);
    QPlainTextEdit* create_label_multi(const QString& label_name);

    std::vector<QWidget*> m_widgets;
    QFont* m_text_font = nullptr;

    const struct HippoMetadataAPI* m_metadata_api = nullptr;
    QTreeView* m_list = nullptr;
    QVBoxLayout* m_main_layout = nullptr;
    QWidget* m_base;
};
