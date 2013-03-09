#include <QDockWidget>
#include <QBoxLayout>

#include "videoimprinter.h"

VideoImprinter::VideoImprinter(QWidget *parent)
    : QMainWindow(parent)
{
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

}

VideoImprinter::~VideoImprinter()
{
    
}
