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

signals:
//    void eventStarted();
//    void eventEnded();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    enum {numOfEventTypes = 10};

    VideoPlayer *videoplayer;
    EventEditor *eventeditor;
    VideoEvent *currentEvent[numOfEventTypes];
    QString eventLabelText[numOfEventTypes];
    bool isEventGoing[numOfEventTypes];
};

#endif // VIDEOIMPRINTER_H
