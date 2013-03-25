#include <QDockWidget>
#include <QBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QStatusBar>
#include <QMessageBox>
#include <QStringBuilder>
#include <QFileInfo>

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

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

//    setCurrentSrtFile(tr(""));
//    setCurrentVideoFile(tr(""));

    // setup event labels
    setEventLabels();
    for (int i=0; i<numOfEventTypes; i++)
    {//TODO: setup label with app settings
        isEventGoing[i] = false;
    }

    //TODO: install event filter with help of childEvent
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

    connect(this, SIGNAL(videoPlayToggled()), videoplayer, SIGNAL(playToggled()));
    connect(eventeditor, SIGNAL(timeDoubleClicked(int))
            , videoplayer, SLOT(setPosition(int)));
//    connect(videoplayer, SIGNAL(positionChanged(qint64))
//            , eventeditor, SLOT(scrollToTime(int)));
    connect(eventeditor, SIGNAL(modificationChanged(bool))
            , this, SLOT(documentWasModified()));


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

void VideoImprinter::sortEvents()
{
    eventeditor->sortEvents();
}

bool VideoImprinter::save()
{
    if (currentSrtFile.isEmpty()) {
        return saveAs();
    } else {
        return saveSrtFile(currentSrtFile);
    }
}

bool VideoImprinter::saveAs()
{
    QFileDialog saveDialog(this, tr("Save Events"));
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.setDirectory(QDir::homePath());
    saveDialog.setNameFilter(tr("Subtitle (*.srt)"));

    saveDialog.exec();

    QStringList fileNames;
    fileNames = saveDialog.selectedFiles();

    if (fileNames.isEmpty())
    {
        statusBar()->showMessage(tr("File Save Canceled."), 2000);
        return false;
    }

    if (saveSrtFile(fileNames.first()) )
    {
        statusBar()->showMessage(tr("File Saved."), 2000);
        return true;
    } else {
        return false;
    }
}

void VideoImprinter::openVideo()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Movie")
                                                    , QDir::homePath()
                                                    , tr("Video (*.*)"));
    if (fileName.isEmpty())
    {
        statusBar()->showMessage(tr("Load canceled"), 2000);
        return;
    }

    if (videoplayer->hasMediaFile())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Video Player"),
                     tr("Already has a media file.\n"
                        "Open a new one?"),
                     QMessageBox::Open | QMessageBox::Cancel);

        if (ret == QMessageBox::Cancel)
            return;
    }

    videoplayer->loadFile(fileName);
    setCurrentVideoFile(fileName);
    statusBar()->showMessage(tr("Movie loaded"), 2000);

    // try to load srt file with same name
    QFileInfo fi(fileName);
    QString srtFileName = fi.filePath() % fi.baseName() % QString(".srt");
    if (!QFile::exists(srtFileName))
    {
        statusBar()->showMessage(tr("No matching event file detected"), 2000);
        return;
    }

    if (maybeSave())
    {
        loadSrtFile(srtFileName);
        setCurrentSrtFile(srtFileName);
        statusBar()->showMessage(tr("Events of the movie loaded"), 2000);
    }
}

void VideoImprinter::openSrt()
{
    if (maybeSave()) {
        //TODO: remeber last path
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Events")
                                                        , QDir::homePath(), tr("Subtitle (*.srt)"));
        if (!fileName.isEmpty())
            loadSrtFile(fileName);

        statusBar()->showMessage(tr("Events loaded"), 2000);
    }
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
        emit videoPlayToggled();
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
        this->save();
        break;
    case Qt::Key_L:
        this->openSrt();
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
        videoplayer->jumpTo(videoJumpSpeeds[0]);
        break;
    case Qt::ControlModifier:
        videoplayer->jumpTo(videoJumpSpeeds[1]);
        break;
    case Qt::ShiftModifier:
        videoplayer->jumpTo(videoJumpSpeeds[2]);
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
        videoplayer->jumpTo(-videoJumpSpeeds[0]);
        break;
    case Qt::ControlModifier:
        videoplayer->jumpTo(-videoJumpSpeeds[1]);
        break;
    case Qt::ShiftModifier:
        videoplayer->jumpTo(-videoJumpSpeeds[2]);
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

void VideoImprinter::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void VideoImprinter::createActions()
{
}

void VideoImprinter::createMenus()
{
}

void VideoImprinter::createToolBars()
{
}


void VideoImprinter::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void VideoImprinter::readSettings()
{
    for (int i=0; i<numOfEventTypes; i++)
    {
        eventLabelText[i] = QString("[%1]").arg(i);
    }

    videoJumpSpeeds[0] = 250;
    videoJumpSpeeds[1] = 1000;
    videoJumpSpeeds[2] = 5000;

}

void VideoImprinter::writeSettings()
{
}

void VideoImprinter::setEventLabels()
{
    for (int i=0; i<numOfEventTypes; i++)
    {
        videoplayer->setEventLabel(i, eventLabelText[i]);
    }

    statusBar()->showMessage(tr("Evnet Labels set."), 2000);
}


bool VideoImprinter::saveSrtFile(const QString filename)
{
    return eventeditor->saveEvents(filename);
}

void VideoImprinter::setCurrentSrtFile(const QString &fileName)
{
    currentSrtFile = fileName;
    eventeditor->setModified(false);
    setWindowModified(false);

    setWindowFilePath(currentSrtFile);
//    eventeditor->setWindowFilePath(currentSrtFile);
}

void VideoImprinter::setCurrentVideoFile(const QString &fileName)
{
    currentVideoFile = fileName;
//    videoplayer->setWindowFilePath(currentVideoFile);
}


bool VideoImprinter::maybeSave()
{
    if (eventeditor->isModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Video Imprinter"),
                     tr("Events has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}


void VideoImprinter::documentWasModified()
{
    setWindowModified(eventeditor->isModified());
}


void VideoImprinter::loadSrtFile(const QString filename)
{
    eventeditor->loadEvents(filename);
}


void VideoImprinter::loadVideoFile(const QString filename)
{
    videoplayer->loadFile(filename);
}

