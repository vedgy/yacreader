#ifndef YACREADER_HISTORY_CONTROLLER_H
#define YACREADER_HISTORY_CONTROLLER_H

#include <QObject>

#include <QModelIndex>
#include <QPersistentModelIndex>

class YACReaderHistoryController;

class YACReaderLibrarySourceContainer
{
public:
    enum SourceType {
        None,
        Folder,
        List
    };

    explicit YACReaderLibrarySourceContainer();
    explicit YACReaderLibrarySourceContainer(const QModelIndex &sourceModelIndex, YACReaderLibrarySourceContainer::SourceType type);
    QModelIndex getSourceModelIndex() const;
    YACReaderLibrarySourceContainer::SourceType getType() const;

    //! The destructor and the comparison operators are the only member functions
    //! that may be called if *this has been invalidated and no longer exists.

    bool operator==(const YACReaderLibrarySourceContainer &other) const;
    bool operator!=(const YACReaderLibrarySourceContainer &other) const;

private:
    bool stillExists() const;

    QPersistentModelIndex sourceModelIndex;
    YACReaderLibrarySourceContainer::SourceType type;
    bool isRootIndex;

    friend class YACReaderHistoryController;
};

Q_DECLARE_METATYPE(YACReaderLibrarySourceContainer)

class YACReaderHistoryController : public QObject
{
    Q_OBJECT
public:
    explicit YACReaderHistoryController(QObject *parent = nullptr);

    //! If the current source container still exists, returns true; otherwise replaces
    //! the no longer existing current source with the root index/source and returns false.
    bool validateCurrentSourceContainer();

signals:
    void enabledForward(bool enabled);
    void enabledBackward(bool enabled);
    void modelIndexSelected(YACReaderLibrarySourceContainer);

public slots:
    void clear();
    void backward();
    void forward();
    void updateHistory(const YACReaderLibrarySourceContainer &source);
    YACReaderLibrarySourceContainer currentSourceContainer();

private:
    struct ExistsAndNotEqualToCurrent;

    //! Removes nonexistent sources and "consecutive" existent duplicates between
    //! currentFolderNavigation and the nearest existent different source before it.
    void validateBackward();
    //! Removes nonexistent sources and "consecutive" existent duplicates between
    //! currentFolderNavigation and the nearest existent different source after it.
    void validateForward();

    int currentFolderNavigation;
    QList<YACReaderLibrarySourceContainer> history;
};

#endif // YACREADER_HISTORY_CONTROLLER_H
