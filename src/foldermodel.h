#ifndef FOLDERMODEL_H
#define FOLDERMODEL_H

#include <QAbstractItemModel>

class AccountsEngine;
class FolderModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum FolderRoles {
        RoleFolderId,
        RoleFolderParentId,
        RoleChangeKey,
        RoleDisplayName
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

private slots:
    void insertingRows(const QModelIndex &parent, int first, int last);
    void insertedRows(const QModelIndex &parent, int first, int last);

private:
    void addModel(QAbstractItemModel *model);
    QAbstractItemModel *modelForRow(int row) const;
    int offsetForModel(QAbstractItemModel *model) const;

    AccountsEngine *m_accountsEngine;
    QList<QAbstractItemModel*> m_models;
    QHash<int, QByteArray> m_roleNames;
};

#endif // FOLDERMODEL_H
