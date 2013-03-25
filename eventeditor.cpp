#include <QBoxLayout>

#include "eventeditor.h"
#include "eventmodel.h"


EventEditor::EventEditor(QWidget *parent) :
    QWidget(parent)
  , modified(true)
{
    QList<VideoEvent *> l;
    videoEventModel = new EventModel(l, this);
    videoEventTable = new QTableView(this);

//    videoEventDelegate = new VideoEventDelegate(this);

    videoEventTable->setModel(videoEventModel);
    videoEventTable->setSortingEnabled(false);
//    videoEventTable->setItemDelegate(videoEventDelegate);
    videoEventTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    videoEventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    videoEventTable->setSelectionMode(QAbstractItemView::SingleSelection);

    //loadEvents("");

    // sample fixatures data
    loadEvents("e:\\play.qt\\VItestResource\\20120322-CH5-03.srt");

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoEventTable);

    setLayout(layout);

    connect(this, SIGNAL(eventAdded(int)), this, SLOT(selectRow(int)));
    connect(this, SIGNAL(eventAdded(int)), this, SLOT(scrollToRow(int)));
    connect(this, SIGNAL(eventAdded(int)), this, SLOT(warnDuplicates()));
    connect(this, SIGNAL(eventDeleted(int)), this, SLOT(selectRow(int)));
    connect(this, SIGNAL(eventDeleted(int)), this, SLOT(warnDuplicates()));

    connect(videoEventModel, SIGNAL(eventSelectionChanged(int))
            , videoEventTable, SLOT(selectRow(int)));
    connect(videoEventTable, SIGNAL(doubleClicked(QModelIndex))
            , this, SLOT(processDoubleClick(QModelIndex)));
    connect(videoEventTable, SIGNAL(activated(QModelIndex))
            , videoEventModel, SLOT(selectEvent(QModelIndex)));
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
    setModified(true);
}

void EventEditor::sortEvents()
{
    videoEventModel->sort();
    setModified(true);
}

void EventEditor::deleteEvent()
{
    int selectedRow = videoEventModel->getSelectedEvent();

    if (selectedRow == -1)
        return;

    videoEventModel->removeRows(selectedRow, 1, QModelIndex());

    emit eventDeleted(selectedRow);
    setModified(true);
}

bool EventEditor::saveEvents(QString filename)
{
    return videoEventModel->saveCurrentEvents(filename);
}

void EventEditor::loadEvents(QString filename)
{
    QList<VideoEvent *> eventList = EventModel::readInSrtFile(filename);

    videoEventModel->loadEventList(eventList);
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
    setModified(true);
}

void EventEditor::changeEndTime(int currentTime)
{
    setUpdatesEnabled(false);
    videoEventModel->changeEndTime(currentTime);
    setUpdatesEnabled(true);
    setModified(true);
}

void EventEditor::changeEventText(QString newText)
{
    setUpdatesEnabled(false);
    videoEventModel->changeEventText(newText);
    setUpdatesEnabled(true);
    setModified(true);
}

bool EventEditor::isModified()
{
    return modified;
}

void EventEditor::setModified(bool m)
{
    modified = m;
    emit modificationChanged(m);
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

void EventEditor::selectRow(int row)
{
    videoEventModel->selectEvent(row);
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
