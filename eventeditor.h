#ifndef EVENTEDITOR_H
#define EVENTEDITOR_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>


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

signals:
    
public slots:
    
private:
    QTableView *videoEventTable;
    EventModel *videoEventModel;
};

#endif // EVENTEDITOR_H
