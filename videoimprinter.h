#ifndef VIDEOIMPRINTER_H
#define VIDEOIMPRINTER_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QAction>
#include <QToolBar>
#include <QTimer>
#include <QDir>
#include <QSplitter>

#include "videoplayer.h"
#include "eventeditor.h"

QT_BEGIN_NAMESPACE
//class QDockWidget;
QT_END_NAMESPACE

/**
 * @brief The VideoImprinter class is the main window of
 * this application.
 * By design it contains one video player and one event editor.
 */
class VideoImprinter : public QMainWindow
{
    Q_OBJECT
    
public:
    VideoImprinter(QWidget *parent = 0);
    ~VideoImprinter();

public slots:

    // add a video event
    void toggleEvent(int iEvent);
    void beginEvent(int iEvent);
    void endEvent(int iEvent);

    void selectPreviousEvent();
    void selectNextEvent();
    void selectCurrentEvent(int currentTime);

    void changeStartTime(int currentTime);
    void changeEndTime(int currentTime);
    void changeEventText(QString newText);

    QDir getWorkingDir();
    void setWorkingDir(QDir dir);

    void sortEvents();

    bool save();
    bool saveAs();
    void autoSave();
    void openVideo();
    void openSrt();
    void newSrt();
    void videoSettings();
    void documentWasModified();
    void toggleLayout();
    void setOrientation(Qt::Orientation ori);

signals:
//    void eventStarted();
//    void eventEnded();
    void videoPlayToggled();
    void videoGoto(int time);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void childEvent(QChildEvent* e);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void keyPressJumpForward(QKeyEvent *event);
    void keyPressJumpBackward(QKeyEvent *event);
    void keyPressReleaseAddEvent(QKeyEvent *event);
    void keyPressDeleteEvent(QKeyEvent *event);

    void closeEvent(QCloseEvent *event);

private:
    enum {numOfEventTypes = 10};

    VideoPlayer *videoplayer;
    EventEditor *eventeditor;

    QToolBar *mainToolbar;
    QAction *openVideoAct;
    QAction *openSrtAct;
    QAction *newSrtAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *videoSettingsAct;

    QTimer *autoSaveTimer;
    QDir workingDir;
    QSplitter *mainSplitter;
    Qt::Orientation layoutOrientation;

    // currently ongoing events, to be added to the editor
    VideoEvent *currentEvent[numOfEventTypes];
    QString eventLabelText[numOfEventTypes];
    bool isEventGoing[numOfEventTypes];
    enum EventAddingMode {SinglePressMode = 0, DoublePressMode = 1};
    EventAddingMode addingMode;

    QString currentVideoFile;
    QString currentSrtFile;

    // quick browsing speeds
    int videoJumpSpeeds[3];

    int autoSaveInterval;

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createVideoSettingsDialog();
    void readSettings();
    void writeSettings();

    void loadVideoFile(const QString filename);
    void loadSrtFile(const QString filename);
    void newSrtFile();
    bool maybeSave();
    bool saveSrtFile(const QString filename);

    void setCurrentSrtFile(const QString &fileName);
    void setCurrentVideoFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    //TODO: find a way to set event labels
    void setEventLabels();
};

#endif // VIDEOIMPRINTER_H
