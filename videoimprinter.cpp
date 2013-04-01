#include <QDockWidget>
#include <QBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QDir>
#include <QStatusBar>
#include <QMessageBox>
#include <QStringBuilder>
#include <QFileInfo>
#include <QSettings>

#include "videoimprinter.h"

VideoImprinter::VideoImprinter(QWidget *parent)
    : QMainWindow(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Video Imprinter[*]"));
    move(100, 100);

    mainSplitter = new QSplitter(this);
    videoplayer = new VideoPlayer(this);
    eventeditor = new EventEditor(this);
    mainSplitter->addWidget(videoplayer);
    mainSplitter->addWidget(eventeditor);
    mainSplitter->setOrientation(Qt::Vertical);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mainSplitter);

    QWidget *centralW = new QWidget(this);
    centralW->setLayout(layout);
    setCentralWidget(centralW);

//    QDockWidget *dock = new QDockWidget(tr("Video Player"), this);
//    videoplayer = new VideoPlayer(dock);
//    dock->setWidget(videoplayer);
//    addDockWidget(Qt::TopDockWidgetArea, dock);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

//    setCurrentSrtFile(tr(""));
//    setCurrentVideoFile(tr(""));

    // setup event labels states
    setEventLabels();
    for (int i=0; i<numOfEventTypes; i++)
    {
        isEventGoing[i] = false;
    }

    //TODO: install event filter with help of childEvent
    eventeditor->installEventFilter(this);
//    eventeditor->getTableView()->viewport()->installEventFilter(this);
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

    // set up auto save
    autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));
    autoSaveTimer->start(autoSaveInterval);

    connect(this, SIGNAL(videoPlayToggled()), videoplayer, SIGNAL(playToggled()));
    connect(eventeditor, SIGNAL(timeDoubleClicked(int))
            , videoplayer, SLOT(setPosition(int)));
    connect(this, SIGNAL(videoGoto(int)), videoplayer, SLOT(setPosition(int)));
    connect(videoplayer, SIGNAL(playerPositionChanged(qint64))
            , eventeditor, SLOT(highlightCurrentRows(qint64)));
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

QDir VideoImprinter::getWorkingDir()
{
    return workingDir;
}

void VideoImprinter::setWorkingDIr(QDir dir)
{
    workingDir = dir;
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
//    QFileDialog saveDialog(this, tr("Save Events"));
//    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
//    saveDialog.setDirectory(QDir::homePath());
//    saveDialog.setNameFilter(tr("Subtitle (*.srt)"));

//    saveDialog.exec();

//    QStringList fileNames;
//    fileNames = saveDialog.selectedFiles();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Events As")
                                                    , workingDir.path()
                                                    , tr("Subtitle (*.srt)"));

    if (fileName.isEmpty())
    {
        statusBar()->showMessage(tr("File Save Canceled."), 2000);
        return false;
    }

    return saveSrtFile(fileName);

}

void VideoImprinter::autoSave()
{
    QString fileName = currentSrtFile;

    if (fileName.isEmpty())
        fileName = workingDir.path() % tr("/unknownevents.srt~");
    else
        fileName = fileName % tr("~");

    if (eventeditor->saveEvents(fileName))
    {
        statusBar()->showMessage(tr("Auto save complete"), 2000);
    } else {
        statusBar()->showMessage(tr("Bad -- auto save failed"), 2000);
    }
}

void VideoImprinter::openVideo()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Movie")
                                                    , workingDir.path()
                                                    , tr("Video (*.*)"));

//    QFileDialog openDialog(this, tr("Load Movie"));
//    openDialog.setAcceptMode(QFileDialog::AcceptOpen);
//    openDialog.setDirectory(QDir::homePath());
//    openDialog.setNameFilter(tr("Video (*.*)"));

//    openDialog.exec();

//    QStringList fileNames;
//    fileNames = openDialog.selectedFiles();
//    QString fileName = fileNames.first();

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

    loadVideoFile(fileName);
    setCurrentVideoFile(fileName);
    statusBar()->showMessage(tr("Movie loaded"), 2000);

    // try to load srt file with same name, if not exists create a new one
    QFileInfo fi(fileName);
    QString srtFileName = fi.absolutePath() % tr("/") % fi.baseName() % QString(".srt");
    if (!QFile::exists(srtFileName))
    {
        if (maybeSave())
        {
            loadSrtFile(srtFileName);
            setCurrentSrtFile(srtFileName);
            statusBar()->showMessage(tr("Created event file for the movie"), 2000);
            return;
        }
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
    if (maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Events")
                                                        , workingDir.path()
                                                        , tr("Subtitle (*.srt)"));
        if (fileName.isEmpty())
            return;

        loadSrtFile(fileName);
        setCurrentSrtFile(fileName);
        statusBar()->showMessage(tr("Events loaded"), 2000);
    }
}

void VideoImprinter::videoSettings()
{
    createVideoSettingsDialog();
}

bool VideoImprinter::eventFilter(QObject *obj, QEvent *ev)
{
    if ((ev->type() == QEvent::KeyPress) || (ev->type() == QEvent::KeyRelease))
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);

        switch(keyEvent->key())
        {
        // video control
        case Qt::Key_Space:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Minus:
        case Qt::Key_Plus:

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

            return false;

        default:
            return QObject::eventFilter(obj, ev);
        }
    } else {
        // standard event processing
        return QObject::eventFilter(obj, ev);
    }
}

void VideoImprinter::childEvent(QChildEvent* e)
{
    if (e->child()->isWidgetType()) {
        if (e->type() == QEvent::ChildAdded) {
            e->child()->installEventFilter(this);
        } else if (e->type() == QEvent::ChildRemoved) {
            e->child()->removeEventFilter(this);
        }
    }

    QWidget::childEvent(e);
}

void VideoImprinter::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    // global management
    case Qt::Key_F12:
        this->toggleLayout();

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
    case Qt::Key_Minus:
        videoplayer->speedDown();
        break;
    case Qt::Key_Plus:
        videoplayer->speedUp();
        break;

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
        this->keyPressReleaseAddEvent(event);
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
    case Qt::Key_G:
        this->sortEvents();
        break;
    case Qt::Key_A:
        emit videoGoto(eventeditor->getSelectedStartTime());
        break;
    case Qt::Key_D:
        emit videoGoto(eventeditor->getSelectedEndTime());
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

void VideoImprinter::keyReleaseEvent(QKeyEvent *event)
{
    if (addingMode == VideoImprinter::DoublePressMode)
        return;

    switch(event->key())
    {
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
        this->keyPressReleaseAddEvent(event);
        break;

    default:
        QMainWindow::keyReleaseEvent(event);
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


void VideoImprinter::keyPressReleaseAddEvent(QKeyEvent *event)
{// add event, or change selected event text

    if (event->isAutoRepeat())
        return;

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

    if ((event->modifiers() == Qt::ControlModifier) && (event->type() == QEvent::KeyPress))
    {
        this->changeEventText(eventLabelText[i]);
        return;
    }

    if ((addingMode == VideoImprinter::DoublePressMode) && (event->type() == QEvent::KeyPress))
    {
        this->toggleEvent(i);

    } else {

        if (event->type() == QEvent::KeyPress)
            this->beginEvent(i);
        else if (event->type() == QEvent::KeyRelease)
            this->endEvent(i);
    }

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
    openSrtAct = new QAction(tr("Open srt"), this);
    openSrtAct->setStatusTip(tr("Load events in srt file"));
    connect(openSrtAct, SIGNAL(triggered()), this, SLOT(openSrt()));

    openVideoAct = new QAction(tr("Open video"), this);
    openVideoAct->setStatusTip((tr("Load video")));
    connect(openVideoAct, SIGNAL(triggered()), this, SLOT(openVideo()));

    saveAct = new QAction(tr("Save"), this);
    saveAct->setStatusTip((tr("Save events")));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save As"), this);
    saveAsAct->setStatusTip((tr("Save events to a new srt file")));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    videoSettingsAct = new QAction(tr("Video Filter"), this);
    videoSettingsAct->setStatusTip((tr("Change video play filter")));
    connect(videoSettingsAct, SIGNAL(triggered()), this, SLOT(videoSettings()));

}

void VideoImprinter::createMenus()
{
}

void VideoImprinter::createToolBars()
{
    mainToolbar = addToolBar(tr("Main Control"));
    mainToolbar->addAction(openVideoAct);
    mainToolbar->addAction(openSrtAct);
    mainToolbar->addAction(saveAct);
    mainToolbar->addAction(saveAsAct);
    mainToolbar->addAction(videoSettingsAct);
    mainToolbar->addAction(exitAct);
}


void VideoImprinter::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void VideoImprinter::createVideoSettingsDialog()
{
    QDialog *videoSettingsDialog = new QDialog(this);
    videoSettingsDialog->setWindowTitle(tr("Video Filters"));
    videoSettingsDialog->setModal(false);

    // brightness
    QLabel *b = new QLabel(tr("Brightness"), videoSettingsDialog);
    QSlider *bs = new QSlider(Qt::Horizontal, videoSettingsDialog);
    bs->setRange(-100, 100);
    bs->setValue(videoplayer->getBrightness());

    connect(bs, SIGNAL(sliderMoved(int)), videoplayer, SLOT(setBrightness(int)));
    connect(videoplayer, SIGNAL(brightnessChanged(int)), bs, SLOT(setValue(int)));

    // contrast
    QLabel *c = new QLabel(tr("Contrast"), videoSettingsDialog);
    QSlider *cs = new QSlider(Qt::Horizontal, videoSettingsDialog);
    cs->setRange(-100, 100);
    cs->setValue(videoplayer->getContrast());

    connect(cs, SIGNAL(sliderMoved(int)), videoplayer, SLOT(setContrast(int)));
    connect(videoplayer, SIGNAL(contrastChanged(int)), cs, SLOT(setValue(int)));

    // hue
    QLabel *h = new QLabel(tr("Hue"), videoSettingsDialog);
    QSlider *hs = new QSlider(Qt::Horizontal, videoSettingsDialog);
    hs->setRange(-100, 100);
    hs->setValue(videoplayer->getHue());

    connect(hs, SIGNAL(sliderMoved(int)), videoplayer, SLOT(setHue(int)));
    connect(videoplayer, SIGNAL(hueChanged(int)), hs, SLOT(setValue(int)));

    // saturation
    QLabel *s = new QLabel(tr("Saturation"), videoSettingsDialog);
    QSlider *ss = new QSlider(Qt::Horizontal, videoSettingsDialog);
    ss->setRange(-100, 100);
    ss->setValue(videoplayer->getSaturation());

    connect(ss, SIGNAL(sliderMoved(int)), videoplayer, SLOT(setSaturation(int)));
    connect(videoplayer, SIGNAL(saturationChanged(int)), ss, SLOT(setValue(int)));

    // group together
    QGridLayout *settingsLayout = new QGridLayout;
    settingsLayout->addWidget(b, 0, 0, 1, 1);
    settingsLayout->addWidget(bs, 0, 1, 1, 1);
    settingsLayout->addWidget(c, 1, 0, 1, 1);
    settingsLayout->addWidget(cs, 1, 1, 1, 1);
    settingsLayout->addWidget(h, 2, 0, 1, 1);
    settingsLayout->addWidget(hs, 2, 1, 1, 1);
    settingsLayout->addWidget(s, 3, 0, 1, 1);
    settingsLayout->addWidget(ss, 3, 1, 1, 1);

    videoSettingsDialog->setLayout(settingsLayout);

    videoSettingsDialog->show();
}

void VideoImprinter::readSettings()
{
    workingDir = QDir::homePath();

    QSettings settings(QDir::currentPath().append("/config.ini"), QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(430, 650)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    setOrientation((Qt::Orientation)(settings.value("orientation", Qt::Vertical).toInt()));
    settings.endGroup();

    settings.beginGroup("EventLabel");
    for (int i=0; i<numOfEventTypes; i++)
    {
        eventLabelText[i] = settings.value(QString("label%1").arg(i)
                                           , QString("%1").arg(i))
                                           .toString();
    }
    addingMode = (VideoImprinter::EventAddingMode)(settings.value(QString("AddingMode"), VideoImprinter::DoublePressMode).toInt());
    settings.endGroup();

    settings.beginGroup("Video");
    int defaultSpeeds[3] = {250, 1000, 5000};
    for (int i=0; i<3; i++)
    {
        videoJumpSpeeds[i] = settings.value(QString("jumpSpeed%1").arg(i)
                                            , defaultSpeeds[i])
                                            .toInt();
    }
    settings.endGroup();

    settings.beginGroup("File");
    autoSaveInterval = settings.value("auto_save_interval", 30000).toInt();
    settings.endGroup();
}

void VideoImprinter::writeSettings()
{
    QSettings settings(QDir::currentPath().append("/config.ini"), QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("orientation", layoutOrientation);
    settings.endGroup();

    settings.beginGroup("EventLabel");
    for (int i=0; i<numOfEventTypes; i++)
    {
        settings.setValue(QString("label%1").arg(i), eventLabelText[i]);
    }
    settings.setValue(QString("AddingMode"), addingMode);
    settings.endGroup();

    settings.beginGroup("Video");
    for (int i=0; i<3; i++)
    {
        settings.setValue(QString("jumpSpeed%1").arg(i), videoJumpSpeeds[i]);
    }
    settings.endGroup();

    settings.beginGroup("File");
    settings.setValue("auto_save_interval", autoSaveInterval);
    settings.endGroup();
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
    if (eventeditor->saveEvents(filename))
    {
        setCurrentSrtFile(filename);
        statusBar()->showMessage(tr("File Saved."), 2000);
        return true;
    }

    return false;
}

void VideoImprinter::setCurrentSrtFile(const QString &fileName)
{
    currentSrtFile = fileName;
    eventeditor->setModified(false);

//    setWindowFilePath(currentSrtFile);
    QString t = tr("Video Imprinter[*] - ")
                % QFileInfo(currentSrtFile).fileName()
                % tr(" / ")
                % QFileInfo(currentVideoFile).fileName();

    setWindowTitle(t);
    setWindowModified(false);
}

void VideoImprinter::setCurrentVideoFile(const QString &fileName)
{
    currentVideoFile = fileName;

//    setWindowFilePath(currentSrtFile);
    QString t = tr("Video Imprinter[*] - ")
                % QFileInfo(currentSrtFile).fileName()
                % tr(" / ")
                % QFileInfo(currentVideoFile).fileName();

    setWindowTitle(t);

    workingDir = QFileInfo(currentVideoFile).dir();
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

void VideoImprinter::toggleLayout()
{
    if (layoutOrientation == Qt::Horizontal)
    {
        layoutOrientation = Qt::Vertical;
    } else if (layoutOrientation == Qt::Vertical)
    {
        layoutOrientation = Qt::Horizontal;
    }

    mainSplitter->setOrientation(layoutOrientation);
}

void VideoImprinter::setOrientation(Qt::Orientation ori)
{
    layoutOrientation = ori;
    mainSplitter->setOrientation(layoutOrientation);
}


void VideoImprinter::loadSrtFile(const QString filename)
{
    eventeditor->loadEvents(filename);
}


void VideoImprinter::loadVideoFile(const QString filename)
{
    videoplayer->loadFile(filename);
}

