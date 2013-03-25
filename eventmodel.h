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
 * Selection is partly handled in this model, maybe move to view class?
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
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

    bool isEventDuplicate(VideoEvent *ve ) const;
    void sort(Qt::SortOrder order = Qt::AscendingOrder);

    // row selection
    int getCurrentEventRow(int currentTime);
    QModelIndex getCurrentEventId(int currentTime);
    int getSelectedEvent();
    int getSelectedStartTime();
    int getSelectedEndTime();
    void changeStartTime(int currentTime);
    void changeEndTime(int currentTime);
    void changeEventText(QString newText);

    // row after sorting
    int rowChangedFrom(int originalRow);

    static QList<VideoEvent *> readInSrtFile(QString filename);
    static bool writeToSrtFile(QList<VideoEvent *> listOfVideoEvents, QString filename);
    static QList<int> detectDuplicates(QList<VideoEvent *> listOfVideoEvents);

public slots:
    void selectEvent(int row);
    void selectEvent(QModelIndex id);
    void selectPreviousEvent();
    void selectNextEvent();
    void selectCurrentEvent(int currentTime);
    void highlightRows(QList<int> rows);
    void warnDuplicates();
    bool saveCurrentEvents(const QString filename);
    void newEventsLoaded();
    void loadEventList(QList<VideoEvent *> eventList);

signals:
    void eventChanged(int row);
    void eventSelectionChanged(int row);
    void duplicateDetected(QList<int>);

private:
    //TODO: maybe a custom list type like QStringList?
    QList<VideoEvent *> listOfEvents;

    // map rows before and after sorting
    // index = after sorting (now), value = before sorting
    QList<int> listOfChangedRow;

    QList<int> listOfHighlightedRow;

    // is a selected row
    int selectedEvent;

};

#endif // EVENTMODEL_H
