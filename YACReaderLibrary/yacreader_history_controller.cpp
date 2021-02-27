#include "yacreader_history_controller.h"

#include <algorithm>

YACReaderHistoryController::YACReaderHistoryController(QObject *parent)
    : QObject(parent)
{
}

bool YACReaderHistoryController::validateCurrentSourceContainer()
{
    auto &source = history[currentFolderNavigation];
    if (source.stillExists())
        return true;
    source.sourceModelIndex = QModelIndex();
    source.isRootIndex = true;

    validateBackward();
    if (currentFolderNavigation == 0)
        emit enabledBackward(false);

    validateForward();
    if (currentFolderNavigation == history.size() - 1)
        emit enabledForward(false);

    return false;
}

void YACReaderHistoryController::clear()
{
    currentFolderNavigation = 0;
    history.clear();
    history.append(YACReaderLibrarySourceContainer(QModelIndex(), YACReaderLibrarySourceContainer::Folder)); //root folder is always the first item

    emit(enabledBackward(false));
    emit(enabledForward(false));
}

void YACReaderHistoryController::backward()
{
    validateBackward();

    if (currentFolderNavigation > 0) {
        currentFolderNavigation--;
        emit(modelIndexSelected(history.at(currentFolderNavigation)));
        emit(enabledForward(true));
    }

    if (currentFolderNavigation == 0)
        emit(enabledBackward(false));
}

void YACReaderHistoryController::forward()
{
    validateForward();

    if (currentFolderNavigation < history.count() - 1) {
        currentFolderNavigation++;
        emit(modelIndexSelected(history.at(currentFolderNavigation)));
        emit(enabledBackward(true));
    }

    if (currentFolderNavigation == history.count() - 1)
        emit(enabledForward(false));
}

void YACReaderHistoryController::updateHistory(const YACReaderLibrarySourceContainer &source)
{
    Q_ASSERT(source.stillExists());

    //remove history from current index
    if (!source.sourceModelIndex.isValid() && history.count() == 1)
        return;

    int numElementsToRemove = history.count() - (currentFolderNavigation + 1);
    while (numElementsToRemove > 0) {
        numElementsToRemove--;
        history.removeLast();
    }

    if (source != history.at(currentFolderNavigation)) {
        history.append(source);

        emit(enabledBackward(true));
        currentFolderNavigation++;
    }

    emit(enabledForward(false));
}

YACReaderLibrarySourceContainer YACReaderHistoryController::currentSourceContainer()
{
    auto source = history.at(currentFolderNavigation);
    Q_ASSERT_X(source.stillExists(), Q_FUNC_INFO,
               "History should be updated with a valid index when the current index is invalidated.");
    return source;
}

struct YACReaderHistoryController::ExistsAndNotEqualToCurrent {
    explicit ExistsAndNotEqualToCurrent(const YACReaderHistoryController &controller)
        : c(controller) { }

    bool operator()(const YACReaderLibrarySourceContainer &source) const
    {
        return source.stillExists() && source != c.history.at(c.currentFolderNavigation);
    }

private:
    const YACReaderHistoryController &c;
};

void YACReaderHistoryController::validateBackward()
{
    Q_ASSERT(history.at(currentFolderNavigation).stillExists());

    const auto rEnd = history.crend();
    const auto previous = rEnd - currentFolderNavigation;
    const auto lastToKeep = std::find_if(previous, rEnd, ExistsAndNotEqualToCurrent { *this });

    if (lastToKeep != previous) {
        const auto firstIndexToErase = rEnd - lastToKeep;
        const auto begin = history.begin();
        history.erase(begin + firstIndexToErase, begin + currentFolderNavigation);
        currentFolderNavigation = firstIndexToErase;
    }
}

void YACReaderHistoryController::validateForward()
{
    Q_ASSERT(history.at(currentFolderNavigation).stillExists());

    const auto next = history.begin() + currentFolderNavigation + 1;
    const auto firstToKeep = std::find_if(next, history.end(), ExistsAndNotEqualToCurrent { *this });

    if (firstToKeep != next)
        history.erase(next, firstToKeep);
}

//------------------------------------------------------------------------------

YACReaderLibrarySourceContainer::YACReaderLibrarySourceContainer()
    : sourceModelIndex(QModelIndex()), type(None), isRootIndex(false)
{
}

YACReaderLibrarySourceContainer::YACReaderLibrarySourceContainer(const QModelIndex &sourceModelIndex, YACReaderLibrarySourceContainer::SourceType type)
    : sourceModelIndex(sourceModelIndex), type(type), isRootIndex(!sourceModelIndex.isValid())
{
    Q_ASSERT(stillExists());
}

QModelIndex YACReaderLibrarySourceContainer::getSourceModelIndex() const
{
    Q_ASSERT(stillExists());
    return sourceModelIndex;
}

YACReaderLibrarySourceContainer::SourceType YACReaderLibrarySourceContainer::getType() const
{
    Q_ASSERT(stillExists());
    return type;
}

bool YACReaderLibrarySourceContainer::operator==(const YACReaderLibrarySourceContainer &other) const
{
    return sourceModelIndex == other.sourceModelIndex && type == other.type && isRootIndex == other.isRootIndex;
}

bool YACReaderLibrarySourceContainer::operator!=(const YACReaderLibrarySourceContainer &other) const
{
    return !(*this == other);
}

bool YACReaderLibrarySourceContainer::stillExists() const
{
    // isRootIndex is true only if sourceModelIndex was invalid in our constructor.
    // We assume that the root index always exists. If sourceModelIndex was valid
    // in the constructor, but now is invalid, then it must have been invalidated/removed.
    // Unfortunately, QPersistentModelIndex's API does not distinguish between initially
    // invalid and invalidated index, hence this custom code.
    return isRootIndex || sourceModelIndex.isValid();
}
