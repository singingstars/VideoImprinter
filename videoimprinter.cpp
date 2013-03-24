#include <QDockWidget>
#include <QBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QStatusBar>

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
    dock->setWidget(videoplayer);
    addDockWidget(Qt::TopDockWidgetArea, dock);

    statusBar()->showMessage(tr("Ready"));

    for (int i=0; i<numOfEventTypes; i++)
    {//TODO: setup label with app settings
        eventLabelText[i] = QString("[%1]").arg(i);
        videoplayer->setEventLabel(i, eventLabelText[i]);
        isEventGoing[i] = false;
    }

    eventeditor->installEventFilter(this);
    QList<QWidget*> widgets = eventeditor->findChildren<QWidget*>();
    foreach(QWidget *widget, widgets)
    {
        widget->installEventFilter(this);
    }

    videoplayer->installEventFilter(this);
    widgets = videoplayer->findChildren<QWidget*>();
    foreach(QWidget *widget, widgets)
    {
        widget->installEventFilter(this);
    }

    connect(eventeditor, SIGNAL(timeDoubleClicked(int))
            , videoplayer, SLOT(setPosition(int)));
//    connect(videoplayer, SIGNAL(positionChanged(qint64))
//            , eventeditor, SLOT(scrollToTime(int)));
}

VideoImprinter::~VideoImprinter()
{
    
}

void VideoImprinter::saveFile()
{//TODO: only prototype
    QFileDialog saveDialog(this, tr("Save Events"));
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.setDirectory(QDir::homePath());
    saveDialog.setNameFilter(tr("Subtitle (*.srt)"));

    if (!saveDialog.exec())
    {
        statusBar()->showMessage(tr("File Save Canceled."));
        return;
    }

    QStringList fileNames;
    fileNames = saveDialog.selectedFiles();

    eventeditor->saveEvents(fileNames.first());
    statusBar()->showMessage(tr("File Saved."));
}

void VideoImprinter::loadFile()
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

void VideoImprinter::sortEvents()
{
    eventeditor->sortEvents();
}

bool VideoImprinter::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);

        switch(keyEvent->key())
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

        // editing video event
        case Qt::Key_Semicolon:
        case Qt::Key_Apostrophe:
        case Qt::Key_Delete:
        case Qt::Key_Enter:

        // save/load file
        case Qt::Key_S:
        case Qt::Key_L:

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

    // add video event
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
        this->keyPressAddEvent(event);
        break;

    // select/modify video event
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
    case Qt::Key_Delete:
        this->keyPressDeleteEvent(event);
        break;
    case Qt::Key_Enter:
        this->sortEvents();
        break;

    // save/load file
    case Qt::Key_S:
        this->saveFile();
        break;
    case Qt::Key_L:
        this->loadFile();
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


void VideoImprinter::keyPressAddEvent(QKeyEvent *event)
{// add event, or change selected event text

    int i = -1;

    switch(event->key())
    {
    case Qt::Key_0:
        i = 0;
        break;
    case Qt::Key_1:
        i = 1;
        break;
    case Qt::Key_2:
        i = 2;
        break;
    case Qt::Key_3:
        i = 3;
        break;
    case Qt::Key_4:
        i = 4;
        break;
    case Qt::Key_5:
        i = 5;
        break;
    case Qt::Key_6:
        i = 6;
        break;
    case Qt::Key_7:
        i = 7;
        break;
    case Qt::Key_8:
        i = 8;
        break;
    case Qt::Key_9:
        i = 9;
        break;
    default:
        return;
    }

    if (event->modifiers() == Qt::ControlModifier)
    {
        this->changeEventText(eventLabelText[i]);
    }

    this->toggleEvent(i);

    return;
}

void VideoImprinter::keyPressDeleteEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Delete) && (event->modifiers() == Qt::ControlModifier))
    {
        eventeditor->deleteEvent();
    }

    return;
}
