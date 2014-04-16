#ifndef FOLDERMODEL_H
#define FOLDERMODEL_H

#include <QAbstractItemModel>

class FolderModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum FolderRoles {
        RoleFolderId = Qt::UserRole,
        RoleFolderParentId,
        RoleChangeKey,
        RoleDepth,
        RoleChildrenCount,
        RoleExpanded,
        RoleIsVisible,
        RoleDisplayName,
        RoleHeaderSection
    };
    explicit FolderModel(QObject *parent = 0);

    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual QHash<int,QByteArray> roleNames() const;

public slots:
    void toggleChildrenExpand(int row);

private slots:
    void insertingRows(const QModelIndex &parent, int first, int last);
    void insertedRows(const QModelIndex &parent, int first, int last);
    void removingRows(const QModelIndex &parent, int first, int last);
    void removedRows(const QModelIndex &parent, int first, int last);
    void slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    QModelIndex indexForModelIndex(const QModelIndex &modelIndex);
    void addModel(QAbstractItemModel *model);
    QAbstractItemModel *modelForRow(int row) const;
    int offsetForModel(QAbstractItemModel *model) const;

    QList<QAbstractItemModel*> m_models;
    QHash<int, QByteArray> m_roleNames;
};

#endif // FOLDERMODEL_H
