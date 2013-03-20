#ifndef VIDEOIMPRINTER_H
#define VIDEOIMPRINTER_H

#include <QMainWindow>
#include <QKeyEvent>

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
    void toggleEvent(int iEvent);
    void beginEvent(int iEvent);
    void endEvent(int iEvent);

    void selectPreviousEvent();
    void selectNextEvent();
    void selectCurrentEvent(int currentTime);

    void changeStartTime(int currentTime);
    void changeEndTime(int currentTime);
    void changeEventText(QString newText);

signals:
//    void eventStarted();
//    void eventEnded();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void keyPressEvent(QKeyEvent *event);
    void keyPressJumpForward(QKeyEvent *event);
    void keyPressJumpBackward(QKeyEvent *event);
    void keyPressChangeEventText(QKeyEvent *event);

private:
    enum {numOfEventTypes = 10};

    VideoPlayer *videoplayer;
    EventEditor *eventeditor;

    // currently ongoing events, to be added to the editor
    VideoEvent *currentEvent[numOfEventTypes];
    QString eventLabelText[numOfEventTypes];
    bool isEventGoing[numOfEventTypes];
};

#endif // VIDEOIMPRINTER_H
