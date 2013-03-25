#ifndef EVENTEDITOR_H
#define EVENTEDITOR_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QList>


#include "eventmodel.h"
//#include "videoeventdelegate.h"

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
    
     void addEvent(VideoEvent *ve);
     void sortEvents();
     void deleteEvent();

     bool saveEvents(QString filename);
     void loadEvents(QString filename);

     // modify selected video event
     void changeStartTime(int currentTime);
     void changeEndTime(int currentTime);
     void changeEventText(QString newText);

     bool isModified();

signals:
     void eventAdded(int row);
     void eventDeleted(int row);
     void timeDoubleClicked(int time);
     void modificationChanged(bool changed);
    
public slots:
     void scrollToTime(int currentTime);
     void scrollToTime(qint64 currentTime);
     void scrollToRow(int row);
     void selectRow(int row);
     void selectPreviousEvent();
     void selectNextEvent();
     void selectCurrentEvent(int currentTime);
     void processDoubleClick(QModelIndex index);
     void warnDuplicates();
     void setModified(bool m = true);

//     void outputDebug();

private:
    QTableView *videoEventTable;
    EventModel *videoEventModel;
//    VideoEventDelegate *videoEventDelegate;

//    QLabel *editorDebug;
    bool modified;
};

#endif // EVENTEDITOR_H
