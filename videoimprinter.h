#ifndef VIDEOIMPRINTER_H
#define VIDEOIMPRINTER_H

#include <QMainWindow>

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

private:
    VideoPlayer *videoplayer;
    EventEditor *eventeditor;
};

#endif // VIDEOIMPRINTER_H
