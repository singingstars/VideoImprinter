#include <QDockWidget>
#include <QBoxLayout>

#include "videoimprinter.h"

VideoImprinter::VideoImprinter(QWidget *parent)
    : QMainWindow(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    QWidget *centralW = new QWidget(this);

    eventeditor = new EventEditor(this);

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

void VideoImprinter::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_0:
        this->toggleEvent(0);
        break;
    case Qt::Key_1:
        this->toggleEvent(1);
        break;
    case Qt::Key_2:
        this->toggleEvent(2);
        break;
    case Qt::Key_3:
        this->toggleEvent(3);
        break;
    case Qt::Key_4:
        this->toggleEvent(4);
        break;
    case Qt::Key_5:
        this->toggleEvent(5);
        break;
    case Qt::Key_6:
        this->toggleEvent(6);
        break;
    case Qt::Key_7:
        this->toggleEvent(7);
        break;
    case Qt::Key_8:
        this->toggleEvent(8);
        break;
    case Qt::Key_9:
        this->toggleEvent(9);
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}
