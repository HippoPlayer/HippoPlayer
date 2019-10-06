#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>


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
    PlaylistModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    inline void add_entry(QString title, QString duration, QString description) {
        PlaylistEntry entry = { title, duration, description };
        m_entries.push_back(entry);
    }

private:

    struct PlaylistEntry {
        QString title;
        QString duration;
        QString description;
    };

    QVector<PlaylistEntry> m_entries;
};
