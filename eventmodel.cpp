#include <QtAlgorithms>
#include <QModelIndex>

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
{
    this->listOfEvents = eventList;
}

int EventModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int EventModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return listOfEvents.count();
}


QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case 0:
            return QString("Start");
        case 1:
            return QString("End");
        case 2:
            return QString("Interval");
        case 3:
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

        if (index.column() == 0)
            {
               if (value.type() == QMetaType::Int)
                   ev->setStartTime(value.toInt());
               else if (value.type() == QMetaType::QTime)
                   ev->setStartTime(value.toTime());
               else
                   return false;
            }
        else if (index.column() == 1)
            {
                if (value.type() == QMetaType::Int)
                    ev->setEndTime(value.toInt());
                else if (value.type() == QMetaType::QTime)
                    ev->setEndTime(value.toTime());
                else
                    return false;
            }
        else if (index.column() == 2)
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
        VideoEvent *ev = new VideoEvent(2000, 0, QString(""));
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

    for (int i = 0; i < rowCount(); ++i)
    {
        int r = listOfSorting.at(i).second;

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
    changePersistentIndexList(changedPersistentIndexesFrom, changedPersistentIndexesTo);

    emit(layoutChanged());
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
         case 0:
             return (currentEvent->getStartQTime()).toString("hh:mm:ss.zzz");
         case 1:
             return (currentEvent->getEndQTime()).toString("hh:mm:ss.zzz");
         case 2:
             return (currentEvent->getQInterval()).toString("hh:mm:ss.zzz");
         case 3:
             return currentEvent->getEventText();
         default:
             return QVariant();
         }
    }
    else
        return QVariant();
}


void EventModel::parseSRT()
{
    //TODO
}
