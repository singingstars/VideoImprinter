#include "eventmodel.h"


EventModel::EventModel(QList<VideoEvent *> eventList, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->eventList = eventList;
}

int EventModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return int(4);
}

int EventModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return eventList.count();
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


QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    if (index.row() >= eventList.size())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
         VideoEvent *currentEvent = eventList.at(index.row());
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
