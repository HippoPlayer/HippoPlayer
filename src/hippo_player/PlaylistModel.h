#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QString>
#include <map>
#include <string>

struct HippoCore;
struct HippoSelectSong;
struct HippoUrlEntry;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
class TreeItem;
class PlaylistModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    PlaylistModel(QObject* parent);
    ~PlaylistModel();

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
private:
    void setupModelData(const QStringList& lines, TreeItem* parent);

    TreeItem* rootItem;
};
*/

class PlaylistModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    PlaylistModel(HippoCore* core, QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

private:

    struct StringLen {
        char* text;
        int len;
    };

    QString get_entry(int row, int col) const;

    /*
    struct CmpString {
        bool operator()(const StringLen& a, const StringLen& b) const {
            char temp[1000] = { 0 };
            memcpy(temp, a.text, a.len);
            char temp2[1000] = { 0 };
            memcpy(temp2, b.text, b.len);

            if (a.len != b.len) {
                return false;
            }

            if (strncmp(a.text, b.text, a.len) == 0) {
                printf("%s and %s matches\n", temp, temp2);
                return true;
            }

            return false;
        }
    };
    */

    // Have a cache for strings to redure recreation of QStrings
    //mutable std::map<StringLen, QString, CmpString> m_qstring_cache;
    mutable std::map<std::string, QString> m_qstring_cache;
    // I really don't like to have this here but will do for now
    HippoCore* m_core = nullptr;
};
