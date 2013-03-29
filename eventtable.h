#ifndef EVENTTABLE_H
#define EVENTTABLE_H

#include <QTableView>
#include <QKeyEvent>


class EventTable : public QTableView
{
    Q_OBJECT
public:
    EventTable(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);
    
signals:
    
public slots:
    
};

#endif // EVENTTABLE_H
