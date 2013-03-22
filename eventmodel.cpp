#include <QRegExp>
#include <QtAlgorithms>
#include <QModelIndex>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QStringList>
#include <QBrush>
#include <QColor>

#include "eventmodel.h"

/**
 * @brief The EventModelLessThan class
 * functor class for custom sorting
 */
class EventModelLessThan
{
public:

    inline EventModelLessThan()
        { }

    inline bool operator()(const QPair<VideoEvent*, int> &l,
                           const QPair<VideoEvent*, int> &r) const
    {
        return *(l.first) < *(r.first);
    }
};

class EventModelGreaterThan
{
public:

    inline EventModelGreaterThan()
        { }

    inline bool operator()(const QPair<VideoEvent*, int> &l,
                           const QPair<VideoEvent*, int> &r) const
    {
        return *(r.first) < *(l.first);
    }
};

EventModel::EventModel(QList<VideoEvent *> eventList, QObject *parent)
    : QAbstractTableModel(parent)
    , selectedEvent(-1)
{
    this->listOfEvents = eventList;

    listOfChangedRow.reserve(rowCount());

    for (int i=0; i<rowCount(); i++)
    {
        listOfChangedRow.append(i);
    }

    connect(this, SIGNAL(duplicateDetected(QList<int>))
            ,this, SLOT(highlightRows(QList<int>)));
}

int EventModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return VideoEvent::numOfFields;
}

int EventModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return listOfEvents.count();
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= listOfEvents.size())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
         VideoEvent *currentEvent = listOfEvents.at(index.row());
         switch(index.column())
         {
         case VideoEvent::StartTimeField:
             return currentEvent->getStartQTime().toString("hh:mm:ss.zzz");
         case VideoEvent::EndTimeField:
             return currentEvent->getEndQTime().toString("hh:mm:ss.zzz");
         case VideoEvent::IntervalField:
             return currentEvent->getQInterval().toString("hh:mm:ss.zzz");
         case VideoEvent::EventTextField:
             return currentEvent->getEventText();
         default:
             return QVariant();
         }
    }

    if (role == Qt::UserRole)
    {
        VideoEvent *currentEvent = listOfEvents.at(index.row());
        switch(index.column())
        {
        case VideoEvent::StartTimeField:
            return currentEvent->getStartTime();
        case VideoEvent::EndTimeField:
            return currentEvent->getEndTime();
        case VideoEvent::IntervalField:
            return currentEvent->getInterval();
        case VideoEvent::EventTextField:
            return currentEvent->getEventText();
        default:
            return QVariant();
        }
    }

    if (role == Qt::BackgroundRole)
    {
        if (listOfHighlightedRow.contains(index.row()))
        {
            return QBrush(QColor(Qt::red));
        }

        return QVariant();
    }

    return QVariant();
}

QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case VideoEvent::StartTimeField:
            return QString("Start");
        case VideoEvent::EndTimeField:
            return QString("End");
        case VideoEvent::IntervalField:
            return QString("Interval");
        case VideoEvent::EventTextField:
            return QString("Event Text");
        default:
            return QVariant();
        }
    }

    if (orientation == Qt::Vertical)
    {
        return (section + 1);
    }

    return QVariant();
}

Qt::ItemFlags EventModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool EventModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        VideoEvent * ev = listOfEvents.value(row);

        if (index.column() == VideoEvent::StartTimeField)
            {
               if (value.type() == QMetaType::Int)
                   ev->setStartTime(value.toInt());
               else if (value.type() == QMetaType::QTime)
                   ev->setStartTime(value.toTime());
               else
                   return false;
            }
        else if (index.column() == VideoEvent::EndTimeField)
            {
                if (value.type() == QMetaType::Int)
                    ev->setEndTime(value.toInt());
                else if (value.type() == QMetaType::QTime)
                    ev->setEndTime(value.toTime());
                else
                    return false;
            }
        else if (index.column() == VideoEvent::EventTextField)
            {
                if (value.type() == QMetaType::QString)
                    ev->setEventText(value.toString());
                else
                    return false;
            }

        listOfEvents.replace(row, ev);
        emit(dataChanged(index, index));

        return true;
    }
    return false;
}

bool EventModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        VideoEvent *ev = new VideoEvent(0, 2000, QString(""));
        listOfEvents.insert(position, ev);
    }

    endInsertRows();
    return true;
}

bool EventModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        listOfEvents.removeAt(position);
    }

    endRemoveRows();
    return true;
}

/**
 * @brief EventModel::isEventDuplicate detect possible duplicate event
 *          that overlaps in time with same event text.
 * @param videoEvent
 * @return
 */
bool EventModel::isEventDuplicate(VideoEvent *ve) const
{
    for (int i = 0; i < listOfEvents.size(); ++i)
    {
        if (*ve == *(listOfEvents.at(i)))
            return true;
    }

    return false;
}

/**
 * @brief EventModel::sort
 * @param order
 * Reimplement sort() of the model.
 */
void EventModel::sort(Qt::SortOrder order)
{
    emit(layoutAboutToBeChanged());

    QVector<QPair<VideoEvent*, int> > listOfSorting;

    listOfSorting.reserve(rowCount());


    for (int row = 0; row < rowCount(); ++row)
    {
        VideoEvent *ve = listOfEvents.at(row);
        listOfSorting.append(QPair<VideoEvent*, int>(ve, row));
    }

    if (order == Qt::AscendingOrder)
    {
        EventModelLessThan lt;
        qStableSort(listOfSorting.begin(), listOfSorting.end(), lt);
    }
    else
    {
        EventModelGreaterThan gt;
        qStableSort(listOfSorting.begin(), listOfSorting.end(), gt);
    }

    QList<VideoEvent *> sorted_events;
    sorted_events.reserve(rowCount());

    QModelIndexList changedPersistentIndexesFrom, changedPersistentIndexesTo;
    QList<int> changedRows;

    for (int i = 0; i < rowCount(); ++i)
    {
        int r = listOfSorting.at(i).second;

        changedRows.append(r);

        sorted_events.append(listOfSorting.at(i).first);

        for (int c = 0; c < columnCount(); ++c)
        {
            QModelIndex from = index(r, c, QModelIndex());
            QModelIndex to = index(i, c, QModelIndex());

            changedPersistentIndexesFrom.append(from);
            changedPersistentIndexesTo.append(to);
        }
    }

    listOfEvents = sorted_events;
    listOfChangedRow = changedRows;
    changePersistentIndexList(changedPersistentIndexesFrom, changedPersistentIndexesTo);

    emit(layoutChanged());
}

int EventModel::getCurrentEventRow(int currentTime)
{
    if (!rowCount())
        return -1;

    int maxId = 0;
    int maxTime = (listOfEvents.at(0))->getStartTime();

    for (int i=0; i<rowCount(); i++)
    {
        int ithTime = (listOfEvents.at(i))->getStartTime();

        if ((ithTime < currentTime) && (ithTime > maxTime))
        {
            maxId = i;
            maxTime = ithTime;
        }
    }

    return maxId;
}

QModelIndex EventModel::getCurrentEventId(int currentTime)
{
    if (!rowCount())
        return QModelIndex();

    int maxId = 0;
    int maxTime = (listOfEvents.at(0))->getStartTime();

    for (int i=0; i<rowCount(); i++)
    {
        int ithTime = (listOfEvents.at(i))->getStartTime();

        if ((ithTime < currentTime) && (ithTime > maxTime))
        {
            maxId = i;
            maxTime = ithTime;
        }
    }

    return index(maxId, VideoEvent::StartTimeField, QModelIndex());
}

int EventModel::getSelectedEvent()
{
    return selectedEvent;
}

void EventModel::selectPreviousEvent()
{
    if (selectedEvent < 0)
        return;

    if (selectedEvent > 0)
        selectedEvent--;

    emit eventSelectionChanged(selectedEvent);
}

void EventModel::selectNextEvent()
{
    if (selectedEvent < 0)
        return;

    if (selectedEvent < (rowCount() - 1))
        selectedEvent++;

    emit eventSelectionChanged(selectedEvent);
}

void EventModel::selectCurrentEvent(int currentTime)
{
    QModelIndex currentId = getCurrentEventId(currentTime);

    selectedEvent = currentId.row();

    emit eventSelectionChanged(selectedEvent);
}

void EventModel::highlightRows(QList<int> rows)
{
    listOfHighlightedRow = rows;

    foreach (int row, rows)
    {
        emit dataChanged(index(row, 0, QModelIndex())
                         , index(row, columnCount()-1, QModelIndex()));
    }
}

void EventModel::changeStartTime(int currentTime)
{
    if (selectedEvent < 0)
        return;

    QModelIndex id = index(selectedEvent, VideoEvent::StartTimeField, QModelIndex());
    setData(id, currentTime, Qt::EditRole);
}

void EventModel::changeEndTime(int currentTime)
{
    if (selectedEvent < 0)
        return;

    QModelIndex id = index(selectedEvent, VideoEvent::EndTimeField, QModelIndex());
    setData(id, currentTime, Qt::EditRole);
}

void EventModel::changeEventText(QString newText)
{
    if (selectedEvent < 0)
        return;

    QModelIndex id = index(selectedEvent, VideoEvent::EventTextField, QModelIndex());
    setData(id, newText, Qt::EditRole);
}

int EventModel::rowChangedFrom(int originalRow)
{
    return listOfChangedRow.indexOf(originalRow);
}

void EventModel::warnDuplicates()
{
    QList<int> dups = EventModel::detectDuplicates(listOfEvents);

    emit duplicateDetected(dups);
}

/**
 * @brief EventModel::readInSrtFile
 * @param filename
 * Reads in local .srt file to a event list. Only one line of text is read.
 */
QList<VideoEvent *> EventModel::readInSrtFile(QString filename)
{
    QList<VideoEvent *> listOfVideoEvents;

    QRegExp rxFilename(".*[.]srt$", Qt::CaseInsensitive);
    if (!rxFilename.exactMatch(filename))
        return listOfVideoEvents;

    if (!QFile::exists(filename))
        return listOfVideoEvents;

    QFile srtFile(filename);
    if (!srtFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return listOfVideoEvents;

    QRegExp rxTimeLine("^[0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3} --> [0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3}");
    QRegExp rxStartEndTimeSep(" --> ");
    QRegExp rxTimeSep("[:,]");

    QTextStream in(&srtFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        // Go through lines, until finding the time line
        while (!line.contains(rxTimeLine))
        {
            if (in.atEnd())
                break;
            else
                line = in.readLine();
        }

        QStringList srtTimeStringList = line.split(rxStartEndTimeSep);

        QStringList timeStringList = srtTimeStringList.at(0).split(rxTimeSep);
        QTime startQTime = QTime(timeStringList.at(0).toInt(), timeStringList.at(1).toInt()
                           , timeStringList.at(2).toInt(), timeStringList.at(3).toInt());

        timeStringList = srtTimeStringList.at(1).split(rxTimeSep);
        QTime endQTime = QTime(timeStringList.at(0).toInt(), timeStringList.at(1).toInt()
                           , timeStringList.at(2).toInt(), timeStringList.at(3).toInt());

        QString eventText;
        if (!in.atEnd())
        {
            eventText = in.readLine();

            if (eventText.isEmpty())
                eventText = QString("");

            VideoEvent *ve = new VideoEvent();
            ve->setEndTime(endQTime);
            ve->setStartTime(startQTime);
            ve->setEventText(eventText);;

            listOfVideoEvents.append(ve);
        }
    }

    return listOfVideoEvents;
}

void EventModel::writeToSrtFile(QList<VideoEvent *> listOfVideoEvents, QString filename)
{//TODO: test this function
    if (listOfVideoEvents.size() == 0)
        return;

    QFile srtFile(filename);
    if (!srtFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&srtFile);

    for (int i=0; i < listOfVideoEvents.size(); i++)
    {
        // ID at 1st line
        out << i+1 << endl;

        // Time at 2nd line
        out << listOfVideoEvents.at(i)->getStartQTime().toString("HH:mm:ss,zzz")
            << " --> "
            << listOfVideoEvents.at(i)->getEndQTime().toString("HH:mm:ss,zzz")
            << endl;

        // Event text (subtitle, generally speaking) at 3rd line
        out << listOfVideoEvents.at(i)->getEventText() << endl;

    }

    out << endl;

}

/**
 * @brief EventModel::detectDuplicates
 * @param listOfVideoEvents
 * @return Returns a list of duplicate ids (rows)
 */
QList<int> EventModel::detectDuplicates(QList<VideoEvent *> listOfVideoEvents)
{// qlist.count and qlist.indexof requires deferenced variables, not pointers, so not using them

    QList<int> listOfDuplicateIds;
    listOfDuplicateIds.reserve(listOfVideoEvents.size());

    // check for duplicates one by one
    for (int currentId = 0; currentId < listOfVideoEvents.size(); currentId++)
    {
        // if already in the list, skip all below to next id
        if (listOfDuplicateIds.contains(currentId))
                continue;

        // if current has duplicate then it also counts, mark it false first
        bool isCurrentHasDuplicate = false;

        VideoEvent *ve = listOfVideoEvents.at(currentId);

        // search for the rest of list one by one
        for (int iDup = currentId + 1; iDup < listOfVideoEvents.size(); iDup++)
        {
            VideoEvent *veDup = listOfVideoEvents.at(iDup);

            // operator== only get called for dereferenced variables
            if (*ve == *veDup)
            {
                isCurrentHasDuplicate = true;
                listOfDuplicateIds.append(iDup);
            }
        }

        if (isCurrentHasDuplicate)
            listOfDuplicateIds.append(currentId);

    }

    return listOfDuplicateIds;
}
