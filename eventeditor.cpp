#include <QBoxLayout>
#include "eventeditor.h"
#include "eventmodel.h"


EventEditor::EventEditor(QWidget *parent) :
    QWidget(parent)
{
    // some sample fixture data
    QString text1 = "latency";
    QString text2 = "courtship";
    VideoEvent *one = new VideoEvent(532, 2647, text1);
    VideoEvent *two = new VideoEvent(2647, 5235, text2);
    VideoEvent *three = new VideoEvent(9632, 15236, text2);
    QList<VideoEvent *> eventList;
    eventList << one << two << three;

    videoEventModel = new EventModel(eventList, parent);

    videoEventTable = new QTableView(this);
    videoEventTable->setModel(videoEventModel);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoEventTable);

    setLayout(layout);
}

EventEditor::~EventEditor()
{

}
