#ifndef READING_LIST_MODEL_H
#define READING_LIST_MODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>

#include "yacreader_global.h"

class LabelItem;
class SpecialListItem;
class ReadingListItem;
class ReadingListSeparatorItem;

class ReadingListModelProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ReadingListModelProxy(QObject *parent = nullptr);
};

class ReadingListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ReadingListModel(QObject *parent = nullptr);

    //QAbstractItemModel methods
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    //Convenience methods
    void setupReadingListsData(QString path);
    void addNewLabel(const QString &name, YACReader::LabelColors color);
    void addReadingList(const QString &name); //top level reading list
    void addReadingListAt(const QString &name, const QModelIndex &mi);
    bool isEditable(const QModelIndex &mi);
    bool isReadingList(const QModelIndex &mi);
    bool isReadingSubList(const QModelIndex &mi);
    QString name(const QModelIndex &mi);
    void rename(const QModelIndex &mi, const QString &name);
    void deleteItem(const QModelIndex &mi);
    const QList<LabelItem *> getLabels();

    enum Roles {
        TypeListsRole = Qt::UserRole + 1,
        IDRole,
        LabelColorRole,
        SpecialListTypeRole
    };

    enum TypeList {
        SpecialList,
        Label,
        ReadingList,
        Separator
    };

    enum TypeSpecialList {
        Reading,
        Favorites
    };

signals:

    void addComicsToFavorites(const QList<qulonglong> &comicIds);
    void addComicsToLabel(const QList<qulonglong> &comicIds, qulonglong labelId);
    void addComicsToReadingList(const QList<qulonglong> &comicIds, qulonglong readingListId);

private:
    void cleanAll();
    void setupReadingListsData(QSqlQuery &sqlquery, ReadingListItem *parent);
    QList<SpecialListItem *> setupSpecialLists(QSqlDatabase &db);
    void setupLabels(QSqlDatabase &db);
    void setupReadingLists(QSqlDatabase &db);
    void insertLabelIntoList(LabelItem *item);
    //! @return The position at which insertLabelIntoList(item) would insert an item
    //! with color @p itemColor and name @p itemName.
    int labelInsertionPosition(YACReader::LabelColors itemColor, const QString &itemName) const;
    //! @return The position at which insertLabelIntoList(item) would insert @p item.
    int labelInsertionPosition(const LabelItem *item) const;
    void insertLabelIntoList(LabelItem *item, int pos);
    //! @brief Moves the label at @p sourcePos just before the label at @p destinationPos
    //! or to the end of the list if @p destinationPos equals labels.size().
    //! @note The destination position here is off by one compared to QList::move() when moving down.
    void moveLabel(int sourcePos, int destinationPos);
    void reorderingChildren(QList<ReadingListItem *> children);

    int firstSeparatorRow() const;
    int firstLabelRow() const;
    int secondSeparatorRow() const;
    int firstReadingListRow() const;

    bool rowIsSpecialList(int row, const QModelIndex &parent = QModelIndex()) const;
    bool rowIsLabel(int row, const QModelIndex &parent = QModelIndex()) const;
    bool rowIsReadingList(int row, const QModelIndex &parent = QModelIndex()) const;
    bool rowIsSeparator(int row, const QModelIndex &parent = QModelIndex()) const;

    bool dropComics(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    bool dropSublist(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    //Special lists
    QList<SpecialListItem *> specialLists;

    //Label
    QList<LabelItem *> labels;

    //Reading lists
    ReadingListItem *rootItem; //
    QMap<unsigned long long int, ReadingListItem *> items; //lists relationship

    //separators
    ReadingListSeparatorItem *separator1;
    ReadingListSeparatorItem *separator2;

    QString _databasePath;
};

#endif // READING_LIST_MODEL_H
