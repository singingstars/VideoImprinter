#ifndef EVENTEDITOR_H
#define EVENTEDITOR_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QLabel>


#include "eventmodel.h"

/**
 * @brief The EventEditor class is a 'view' for displaying video events.
 *
 * Using Qt's model-view pattern, EventEditor class is a widget for
 * viewing and editing video events. Each event (similar to subtile) has
 * a begin time, an end time, an interval, and event type (equivalent to
 * subtitle text).
 */
class EventEditor : public QWidget
{
    Q_OBJECT
public:
     EventEditor(QWidget *parent = 0);
    ~EventEditor();
    
     bool addEvent(VideoEvent *ve);
     void sortEvents();

     // modify selected video event
     void changeStartTime(int currentTime);
     void changeEndTime(int currentTime);
     void changeEventText(QString newText);

signals:
     void eventAdded(int time);
    
public slots:
     void focusEvent(int currentTime);
     void selectPreviousEvent();
     void selectNextEvent();
     void selectCurrentEvent(int currentTime);

//     void outputDebug();

private:
    QTableView *videoEventTable;
    EventModel *videoEventModel;

//    QLabel *editorDebug;
};

#endif // EVENTEDITOR_H
