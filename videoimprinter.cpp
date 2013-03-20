#include <QDockWidget>
#include <QBoxLayout>

#include "videoimprinter.h"

VideoImprinter::VideoImprinter(QWidget *parent)
    : QMainWindow(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    QWidget *centralW = new QWidget(this);

    eventeditor = new EventEditor(this);
    eventeditor->installEventFilter(this);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(eventeditor);

    centralW->setLayout(layout);
    setCentralWidget(centralW);

    QDockWidget *dock = new QDockWidget(tr("Video Player"), this);
    videoplayer = new VideoPlayer(dock);
    for (int i=0; i<numOfEventTypes; i++)
    {
        eventLabelText[i] = QString("[%1]").arg(i);
        videoplayer->setEventLabel(i, eventLabelText[i]);
        isEventGoing[i] = false;
    }
    dock->setWidget(videoplayer);
    addDockWidget(Qt::TopDockWidgetArea, dock);

    connect(eventeditor, SIGNAL(timeDoubleClicked(int))
            , videoplayer, SLOT(setPosition(int)));
//    connect(videoplayer, SIGNAL(positionChanged(qint64))
//            , eventeditor, SLOT(scrollToTime(int)));
}

VideoImprinter::~VideoImprinter()
{
    
}

void VideoImprinter::toggleEvent(int iEvent)
{
    if (isEventGoing[iEvent])
        this->endEvent(iEvent);
    else
        this->beginEvent(iEvent);
}

void VideoImprinter::beginEvent(int iEvent)
{
    currentEvent[iEvent] = new VideoEvent(videoplayer->getPosition(),
                                          videoplayer->getPosition(),
                                          eventLabelText[iEvent]);
    isEventGoing[iEvent] = true;
    videoplayer->highlightEventText(iEvent, true);
}

void VideoImprinter::endEvent(int iEvent)
{
    currentEvent[iEvent]->setEndTime(videoplayer->getPosition());
    isEventGoing[iEvent] = false;
    eventeditor->addEvent(currentEvent[iEvent]);
    videoplayer->highlightEventText(iEvent, false);
}

void VideoImprinter::selectPreviousEvent()
{
    eventeditor->selectPreviousEvent();
}

void VideoImprinter::selectNextEvent()
{
    eventeditor->selectNextEvent();
}

void VideoImprinter::selectCurrentEvent(int currentTime)
{
    eventeditor->selectCurrentEvent(currentTime);
}

void VideoImprinter::changeStartTime(int currentTime)
{
    eventeditor->changeStartTime(currentTime);
}

void VideoImprinter::changeEndTime(int currentTime)
{
    eventeditor->changeEndTime(currentTime);
}

void VideoImprinter::changeEventText(QString newText)
{
    eventeditor->changeEventText(newText);
}

bool VideoImprinter::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);

        switch(keyEvent->key())
        {
        case Qt::Key_Space:
        case Qt::Key_Left:
        case Qt::Key_Right:
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
        case Qt::Key_BracketLeft:
        case Qt::Key_BracketRight:
        case Qt::Key_P:
        case Qt::Key_Semicolon:
        case Qt::Key_Apostrophe:
            return false;
        default:
            return QObject::eventFilter(obj, ev);
        }
    } else {
        // standard event processing
        return QObject::eventFilter(obj, ev);
    }
}

void VideoImprinter::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    // video player controls
    case Qt::Key_Space:
        videoplayer->play();
        break;
    case Qt::Key_Left:
        this->keyPressJumpBackward(event);
        break;
    case Qt::Key_Right:
        this->keyPressJumpForward(event);
        break;
//    case Qt::Key_Down:
//        videoplayer->speedDown();
//        break;
//    case Qt::Key_Up:
//        videoplayer->speedUp();
//        break;

    // event controls/modifiers
    case Qt::Key_0:
        if (event->modifiers() == Qt::ControlModifier)
        {
            this->changeEventText(eventLabelText[0]);
            break;
        }
        this->toggleEvent(0);
        break;
    case Qt::Key_1:
        if (event->modifiers() == Qt::ControlModifier)
        {
            this->changeEventText(eventLabelText[1]);
            break;
        }
        this->toggleEvent(1);
        break;
    case Qt::Key_2:
        if (event->modifiers() == Qt::ControlModifier)
        {
            this->changeEventText(eventLabelText[2]);
            break;
        }
        this->toggleEvent(2);
        break;
    case Qt::Key_3:
        if (event->modifiers() == Qt::ShiftModifier)
        {
            this->changeEventText(eventLabelText[3]);
            break;
        }
        this->toggleEvent(3);
        break;
    case Qt::Key_4:
        if (event->modifiers() == Qt::ShiftModifier)
        {
            this->changeEventText(eventLabelText[4]);
            break;
        }
        this->toggleEvent(4);
        break;
    case Qt::Key_5:
        if (event->modifiers() == Qt::ShiftModifier)
        {
            this->changeEventText(eventLabelText[5]);
            break;
        }
        this->toggleEvent(5);
        break;
    case Qt::Key_6:
        if (event->modifiers() == Qt::ShiftModifier)
        {
            this->changeEventText(eventLabelText[6]);
            break;
        }
        this->toggleEvent(6);
        break;
    case Qt::Key_7:
        if (event->modifiers() == Qt::ShiftModifier)
        {
            this->changeEventText(eventLabelText[7]);
            break;
        }
        this->toggleEvent(7);
        break;
    case Qt::Key_8:
        if (event->modifiers() == Qt::ShiftModifier)
        {
            this->changeEventText(eventLabelText[8]);
            break;
        }
        this->toggleEvent(8);
        break;
    case Qt::Key_9:
        if (event->modifiers() == Qt::ShiftModifier)
        {
            this->changeEventText(eventLabelText[9]);
            break;
        }
        this->toggleEvent(9);
        break;
    case Qt::Key_BracketLeft:
        this->selectPreviousEvent();
        break;
    case Qt::Key_BracketRight:
        this->selectNextEvent();
        break;
    case Qt::Key_P:
        this->selectCurrentEvent(videoplayer->getPosition());
        break;
    case Qt::Key_Semicolon:
        this->changeStartTime(videoplayer->getPosition());
        break;
    case Qt::Key_Apostrophe:
        this->changeEndTime(videoplayer->getPosition());
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void VideoImprinter::keyPressJumpForward(QKeyEvent *event)
{
    //TODO: use settings instead of magic numbers
    switch(event->modifiers())
    {
    case Qt::NoModifier:
        videoplayer->jumpTo(250);
        break;
    case Qt::ControlModifier:
        videoplayer->jumpTo(1000);
        break;
    case Qt::ShiftModifier:
        videoplayer->jumpTo(5000);
        break;
    default:
        return;
    }
}

void VideoImprinter::keyPressJumpBackward(QKeyEvent *event)
{
    //TODO: use settings instead of magic numbers
    switch(event->modifiers())
    {
    case Qt::NoModifier:
        videoplayer->jumpTo(-250);
        break;
    case Qt::ControlModifier:
        videoplayer->jumpTo(-1000);
        break;
    case Qt::ShiftModifier:
        videoplayer->jumpTo(-5000);
        break;
    default:
        return;
    }
}
