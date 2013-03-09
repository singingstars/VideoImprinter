#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QVariant>

#include "videoevent.h"

/**
 * @brief The EventModel class is a model representing video events.
 *
 * Using model-view design pattern in Qt, video events (somewhat similar to subtitles)
 * needs a dedicated model. It holds a list of events, and contain columns representing
 * begin time, end time, interval, and event type (equivalent to subtitle text).
 * It also contains methods to parse .srt file as well as to write to it.
 */
class EventModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    EventModel(QList<VideoEvent *> eventList, QObject *parent = 0);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

signals:
    
public slots:

private:
    QList<VideoEvent *> eventList;

    void parseSRT();

};

#endif // EVENTMODEL_H
