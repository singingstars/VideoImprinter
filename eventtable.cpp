#include "eventtable.h"

EventTable::EventTable(QWidget *parent) :
    QTableView(parent)
{
}

void EventTable::keyPressEvent(QKeyEvent *event)
{// pass some key press to parent
    switch(event->key())
    {
    // video control
    case Qt::Key_Space:
    case Qt::Key_Left:
    case Qt::Key_Right:

    // adding video event
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:

    // select video event from list
    case Qt::Key_BracketLeft:
    case Qt::Key_BracketRight:
    case Qt::Key_P:
    case Qt::Key_A:
    case Qt::Key_D:

    // editing video event
    case Qt::Key_Semicolon:
    case Qt::Key_Apostrophe:
    case Qt::Key_Delete:
    case Qt::Key_G:

    // save/load file
    case Qt::Key_S:
    case Qt::Key_L:

        event->ignore();

    default:
        QTableView::keyPressEvent(event);
    }
}
