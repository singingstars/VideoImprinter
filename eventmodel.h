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
    EventModel(QList<VideoEvent *> listOfEvents, QObject *parent = 0);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

    bool isEventDuplicate(VideoEvent *ve ) const;
    void sort(Qt::SortOrder order = Qt::AscendingOrder);

    QModelIndex getCurrentEventId(int currentTime);
    int getSelectedEvent();
    void changeStartTime(int currentTime);
    void changeEndTime(int currentTime);
    void changeEventText(QString newText);

public slots:
    void selectPreviousEvent();
    void selectNextEvent();
    void selectCurrentEvent(int currentTime);

signals:
    void eventSelectionChanged(int row);
    
private:
    //TODO: maybe a custom list type like QStringList?
    QList<VideoEvent *> listOfEvents;

    // is a selected row
    int selectedEvent;

    void parseSRT();

};

#endif // EVENTMODEL_H
