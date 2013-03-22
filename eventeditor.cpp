#include <QBoxLayout>

#include "eventeditor.h"
#include "eventmodel.h"


EventEditor::EventEditor(QWidget *parent) :
    QWidget(parent)
{
    QList<VideoEvent *> eventList;

    eventList = EventModel::readInSrtFile("e:\\play.qt\\VItestResource\\20120322-CH5-03.srt");

    videoEventModel = new EventModel(eventList, parent);

//    videoEventDelegate = new VideoEventDelegate(this);

    videoEventTable = new QTableView(this);
    videoEventTable->setModel(videoEventModel);
    videoEventTable->setSortingEnabled(false);
//    videoEventTable->setItemDelegate(videoEventDelegate);
    videoEventTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    videoEventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    videoEventTable->setSelectionMode(QAbstractItemView::SingleSelection);

    warnDuplicates();

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

    connect(this, SIGNAL(eventAdded(int)), this, SLOT(scrollToRow(int)));
    connect(this, SIGNAL(eventAdded(int)), this, SLOT(warnDuplicates()));
    connect(videoEventModel, SIGNAL(eventSelectionChanged(int))
            , videoEventTable, SLOT(selectRow(int)));
    connect(videoEventTable, SIGNAL(doubleClicked(QModelIndex))
            , this, SLOT(processDoubleClick(QModelIndex)));
    connect(this, SIGNAL(duplicatesDetected(QList<int>))
            , videoEventModel, SLOT(highlightRows(QList<int>)));
}

EventEditor::~EventEditor()
{

}

void EventEditor::addEvent(VideoEvent *ve)
{// duplication is highlighted to notify the user
    videoEventModel->insertRows(0, 1, QModelIndex());

    // must set end time first; otherwise start time > end time, will be swaped
    QModelIndex index;
    index = videoEventModel->index(0, VideoEvent::EndTimeField, QModelIndex());
    videoEventModel->setData(index, ve->getEndTime(), Qt::EditRole);

    index = videoEventModel->index(0, VideoEvent::StartTimeField, QModelIndex());
    videoEventModel->setData(index, ve->getStartTime(), Qt::EditRole);

    index = videoEventModel->index(0, VideoEvent::EventTextField, QModelIndex());
    videoEventModel->setData(index, ve->getEventText(), Qt::EditRole);

    videoEventModel->sort();

    emit eventAdded(videoEventModel->rowChangedFrom(0));
}

void EventEditor::sortEvents()
{
    videoEventModel->sort();
}

void EventEditor::warnDuplicates()
{
    videoEventModel->warnDuplicates();
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

void EventEditor::scrollToTime(int currentTime)
{
    QModelIndex currentId = videoEventModel->getCurrentEventId(currentTime);
    videoEventTable->scrollTo(currentId);
}

void EventEditor::scrollToTime(qint64 currentTime)
{
    currentTime = static_cast<int>(currentTime);
    QModelIndex currentId = videoEventModel->getCurrentEventId(currentTime);
    videoEventTable->scrollTo(currentId);
}

void EventEditor::scrollToRow(int row)
{
    QModelIndex idRow = videoEventModel->index(row, 0, QModelIndex());
    videoEventTable->scrollTo(idRow);
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

void EventEditor::processDoubleClick(QModelIndex index)
{
    if (!index.isValid())
        return;

    if (index.row() >= videoEventModel->rowCount())
        return;

    if ((index.column() == VideoEvent::StartTimeField)
         || (index.column() == VideoEvent::EndTimeField))
    {
        int currentTime = videoEventModel->data(index, Qt::UserRole).toInt();
        emit timeDoubleClicked(currentTime);
    }

    return;
}

//void EventEditor::outputDebug()
//{
//    editorDebug->setText(QString("y-%1").arg(QTime::currentTime().toString()));
//}
