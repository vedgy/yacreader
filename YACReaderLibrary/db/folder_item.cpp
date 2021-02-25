#include <QStringList>
#include <algorithm>

#include "folder_item.h"
#include "qnaturalsorting.h"

FolderItem::FolderItem(const QList<QVariant> &data, FolderItem *parent)
{
    parentItem = parent;
    itemData = data;
}

FolderItem::~FolderItem()
{
    qDeleteAll(childItems);
}

void FolderItem::insertChild(FolderItem *item)
{
    insertChild(item, insertionPosition(item));
}

int FolderItem::insertionPosition(const FolderItem *item) const
{
    const auto getItemName = [](const FolderItem *item) { return item->data(1).toString(); };
    const auto it = std::upper_bound(childItems.cbegin(), childItems.cend(), getItemName(item),
                                     [getItemName](const QString &name, const FolderItem *item) {
                                         return naturalSortLessThanCI(name, getItemName(item));
                                     });
    return it - childItems.cbegin();
}

void FolderItem::insertChild(FolderItem *item, int pos)
{
    item->parentItem = this;
    childItems.insert(pos, item);
}

FolderItem *FolderItem::child(int row)
{
    return childItems.value(row);
}

int FolderItem::childCount() const
{
    return childItems.count();
}

int FolderItem::columnCount() const
{
    return itemData.count();
}

QVariant FolderItem::data(int column) const
{
    return itemData.value(column);
}

void FolderItem::setData(int column, const QVariant &value)
{
    itemData[column] = value;
}

void FolderItem::removeChild(int childIndex)
{
    childItems.removeAt(childIndex);
}

void FolderItem::clearChildren()
{
    qDeleteAll(childItems);
    childItems.clear();
}

QList<FolderItem *> FolderItem::children()
{
    return childItems;
}

FolderItem *FolderItem::parent()
{
    return parentItem;
}

int FolderItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<FolderItem *>(this));

    return 0;
}

QList<QVariant> FolderItem::getData() const
{
    return itemData;
}
