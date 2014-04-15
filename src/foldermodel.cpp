#include "foldermodel.h"

#include "AccountsEngine.h"

#include <QDebug>

FolderModel::FolderModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_roleNames[RoleFolderId]       = "roleFolderId";
    m_roleNames[RoleFolderParentId] = "roleFolderParentId";
    m_roleNames[RoleChangeKey]      = "roleChangeKey";
    m_roleNames[RoleDepth]          = "roleDepth";
    m_roleNames[RoleHasChildren]    = "roleHasChildren";
    m_roleNames[RoleExpanded]       = "roleExpanded";
    m_roleNames[RoleDisplayName]    = "roleDisplayName";

    QList<QAbstractItemModel*> models = AccountsEngine::instance()->engineFolderModels();
    foreach (QAbstractItemModel *model, models) {
        addModel(model);
    }
}

QModelIndex FolderModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(column)

    if (!parent.isValid()) {
        QAbstractItemModel *model = modelForRow(row);
        if (model) {
            return createIndex(row, column, model);
        }
    } else {
        qDebug() << "_index_" << row << parent;
    }
    return QModelIndex();
}

QModelIndex FolderModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

QModelIndex FolderModel::sibling(int row, int column, const QModelIndex &idx) const
{
    return QModelIndex();
}

int FolderModel::rowCount(const QModelIndex &parent) const
{
    int count = 0;
    foreach (QAbstractItemModel *model, m_models) {
        count += model->rowCount();
    }
    return count;
}

int FolderModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

bool FolderModel::hasChildren(const QModelIndex &parent) const
{

}

QVariant FolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.parent().isValid()) {
        QAbstractItemModel *model = modelForRow(index.row());
        int offset = offsetForModel(model);

        QModelIndex modelIndex = model->index(index.row() - offset, 0);
        return model->data(modelIndex, role);
    } else {
        qDebug() << "_index_" << index.row() << index.parent();
        const QAbstractItemModel *model = index.model();
        return model->data(index, role);
    }
}

QHash<int, QByteArray> FolderModel::roleNames() const
{
    return m_roleNames;
}

void FolderModel::toggleChildrenExpand(int row)
{
    QAbstractItemModel *model = modelForRow(row);
    int offset = offsetForModel(model);

    QModelIndex modelIndex = model->index(row - offset, 0);
    bool expanded = modelIndex.data(RoleExpanded).toBool();
    model->setData(modelIndex, !expanded, RoleExpanded);
}

void FolderModel::insertingRows(const QModelIndex &parent, int first, int last)
{
    // The model is inserting rows
    int offset = offsetForModel(qobject_cast<QAbstractItemModel*>(sender()));
    beginInsertRows(QModelIndex(),
                    offset + first,
                    offset + last);
}

void FolderModel::insertedRows(const QModelIndex &parent, int first, int last)
{
    endInsertRows();
}

void FolderModel::slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    QAbstractItemModel *model = qobject_cast<QAbstractListModel*>(sender());
    int offset = offsetForModel(model);
    offset = 0;
    qDebug() << Q_FUNC_INFO << offset;
    emit dataChanged(createIndex(topLeft.row() + offset, 0, model),
                     createIndex(bottomRight.row() + offset, 0, model),
                     roles);
}

void FolderModel::addModel(QAbstractItemModel *model)
{
    qDebug() << model << model->rowCount();

    if (model->rowCount()) {
        beginInsertRows(QModelIndex(),
                        rowCount(),
                        model->rowCount());
    }
    m_models << model;
    connect(model, SIGNAL(modelAboutToBeReset()),
            this, SIGNAL(modelAboutToBeReset()));
    connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(insertingRows(QModelIndex,int,int)));
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(insertedRows(QModelIndex,int,int)));
    connect(model, &QAbstractItemModel::dataChanged,
            this, &FolderModel::slotDataChanged);

    if (model->rowCount()) {
        endInsertRows();
    }
}

QAbstractItemModel *FolderModel::modelForRow(int row) const
{
    int count = 0;
    foreach (QAbstractItemModel *model, m_models) {
        count += model->rowCount();
        if (row < count) {
            return model;
        }
    }
    return 0;
}

int FolderModel::offsetForModel(QAbstractItemModel *itemModel) const
{
    int count = 0;
    foreach (QAbstractItemModel *model, m_models) {
        if (itemModel == model) {
            return count;
        }
        count += model->rowCount();
    }
    return count;
}
