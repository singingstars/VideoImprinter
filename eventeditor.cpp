#include <QBoxLayout>

#include "eventeditor.h"
#include "eventmodel.h"


EventEditor::EventEditor(QWidget *parent) :
    QWidget(parent)
{
    QList<VideoEvent *> eventList;
    videoEventModel = new EventModel(eventList, parent);

    videoEventTable = new QTableView(this);
    videoEventTable->setModel(videoEventModel);
    videoEventTable->setSortingEnabled(false);

    videoEventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    videoEventTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // some sample fixture data
//    QString text1 = "latency";
//    QString text2 = "courtship";
//    VideoEvent *one = new VideoEvent(532, 2647, text1);
//    VideoEvent *two = new VideoEvent(2647, 5235, text2);
//    VideoEvent *three = new VideoEvent(9632, 15236, text2);
//    eventList << one << two << three;

    // test add event from model
   // QModelIndex index;
    QString textadd = "added";
    VideoEvent *added = new VideoEvent(30489, 30489, textadd);
    this->addEvent(added);

//    videoEventModel->sort();

//    index = videoEventModel->index(3,0,QModelIndex());
//    videoEventModel->setData(index, 30382, Qt::EditRole);

    int ss = VideoEvent::msFromQTime(VideoEvent::QTimeFromMs(52567976));
    VideoEvent *added2 = new VideoEvent(45246, ss, textadd);
    this->addEvent(added2);
//    videoEventModel->sort(Qt::DescendingOrder);


    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoEventTable);

    // debug output
//    editorDebug = new QLabel;
//    layout->addWidget(editorDebug);
//    connect(videoEventModel, SIGNAL(dataChanged(QModelIndex,QModelIndex))
//            , this, SLOT(outputDebug()));

    setLayout(layout);

    connect(this, SIGNAL(eventAdded(int)), this, SLOT(focusEvent(int)));
    connect(videoEventModel, SIGNAL(eventSelectionChanged(int))
            , videoEventTable, SLOT(selectRow(int)));
}

EventEditor::~EventEditor()
{

}

bool EventEditor::addEvent(VideoEvent *ve)
{
    if (!(videoEventModel->isEventDuplicate(ve)))
    {
        videoEventModel->insertRows(0, 1, QModelIndex());

        // must set end time first; otherwise can't set start time; don't know why
        QModelIndex index;
        index = videoEventModel->index(0, 1, QModelIndex());
        videoEventModel->setData(index, ve->getEndTime(), Qt::EditRole);

        index = videoEventModel->index(0, 0, QModelIndex());
        videoEventModel->setData(index, ve->getStartTime(), Qt::EditRole);

        index = videoEventModel->index(0, 2, QModelIndex());
        videoEventModel->setData(index, ve->getEventText(), Qt::EditRole);

        videoEventModel->sort();

        emit eventAdded(ve->getStartTime());
        return true;
    } //TODO: handle duplicate

    return false;
}

void EventEditor::sortEvents()
{

}

void EventEditor::changeStartTime(int currentTime)
{
    setUpdatesEnabled(false);
    videoEventModel->changeStartTime(currentTime);
    setUpdatesEnabled(true);
}

void EventEditor::changeEndTime(int currentTime)
{
    setUpdatesEnabled(false);
    videoEventModel->changeEndTime(currentTime);
    setUpdatesEnabled(true);
}

void EventEditor::changeEventText(QString newText)
{
    setUpdatesEnabled(false);
    videoEventModel->changeEventText(newText);
    setUpdatesEnabled(true);
}

void EventEditor::focusEvent(int currentTime)
{
    QModelIndex currentId = videoEventModel->getCurrentEventId(currentTime);
    videoEventTable->scrollTo(currentId);
}

void EventEditor::selectPreviousEvent()
{
    videoEventModel->selectPreviousEvent();
}

void EventEditor::selectNextEvent()
{
    videoEventModel->selectNextEvent();
}

void EventEditor::selectCurrentEvent(int currentTime)
{
    videoEventModel->selectCurrentEvent(currentTime);
}

//void EventEditor::outputDebug()
//{
//    editorDebug->setText(QString("y-%1").arg(QTime::currentTime().toString()));
//}
