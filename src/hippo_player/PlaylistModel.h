#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>

class TreeItem;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
