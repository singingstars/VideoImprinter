
#include "videoevent.h"

VideoEvent::VideoEvent(QObject *parent)
    : QObject(parent)
{
    this->startTime = 0;
    this->endTime = 0;
    this->eventText = QString("");
}

VideoEvent::VideoEvent(int startTime, int endTime, QString eventText, QObject *parent)
    : QObject(parent)
{
    if (startTime > endTime)
    {
        int time = endTime;
        endTime = startTime;
        startTime = time;
    }

    this->startTime = startTime;
    this->endTime = endTime;
    this->eventText = eventText;
}

VideoEvent::~VideoEvent()
{
}

int VideoEvent::getStartTime() const
{
    return startTime;
}

int VideoEvent::getEndTime() const
{
    return endTime;
}

QString VideoEvent::getEventText() const
{
    return eventText;
}

int VideoEvent::getInterval() const
{
    return (endTime - startTime);
}

QTime VideoEvent::getStartQTime() const
{
    return VideoEvent::QTimeFromMs(startTime);
}

QTime VideoEvent::getEndQTime() const
{
    return VideoEvent::QTimeFromMs(endTime);
}

QTime VideoEvent::getQInterval() const
{
    return VideoEvent::QTimeFromMs((endTime - startTime));
}

void VideoEvent::setStartTime(int time)
{
    if (time < endTime)
        startTime = time;
    else
    {
        startTime = endTime;
        endTime = time;
    }
}

void VideoEvent::setEndTime(int time)
{
    if (time > startTime)
        endTime = time;
    else
    {
        endTime = startTime;
        startTime = time;
    }
}

void VideoEvent::setStartTime(QTime time)
{
    this->setStartTime(VideoEvent::msFromQTime(time));
}

void VideoEvent::setEndTime(QTime time)
{
    this->setEndTime(VideoEvent::msFromQTime(time));
}

void VideoEvent::setEventText(QString text)
{
    this->eventText = text;
}

bool VideoEvent::operator<(const VideoEvent & other)
{
    if (startTime < other.getStartTime())
    {
        return true;
    }
    else if (startTime == other.getStartTime())
    {
        if (endTime < other.getEndTime())
        {
            return true;
        }
    }
    return false;
}

bool VideoEvent::operator==(const VideoEvent &other)
{
    // text must equal
    if (QString::compare(eventText, other.getEventText(), Qt::CaseInsensitive) != 0)
        return false;

    // time has overlap
    // ref. http://stackoverflow.com/questions/143552/comparing-date-ranges
    if ((startTime < other.getEndTime()) && (endTime > other.getStartTime()))
        return true;
    else
        return false;
}

/**
 * @brief VideoEvent::QTimeFromMs
 * @param totalmsecs
 * @return
 *
 */
QTime VideoEvent::QTimeFromMs(int totalmsecs)
{
    int hours = totalmsecs / 3600000;
    int minutes = (totalmsecs % 3600000) / 60000;
    int seconds = (totalmsecs % 60000) / 1000;
    int mseconds = totalmsecs % 1000;

    return QTime(hours, minutes, seconds, mseconds);
}

QTime VideoEvent::QTimeFromMs(qint64 totalmsecs)
{
    qint64 hours = totalmsecs / 3600000;
    qint64 minutes = (totalmsecs % 3600000) / 60000;
    qint64 seconds = (totalmsecs % 60000) / 1000;
    qint64 mseconds = totalmsecs % 1000;

    return QTime(hours, minutes, seconds, mseconds);
}

/**
 * @brief VideoEvent::msFromQTime
 * @param timeObject
 * @return
 *
 *  copied from ksubtitle project's csubtitles.cpp, author Tom Deblauwe
 */
int VideoEvent::msFromQTime(QTime timeObject)
{
    return ((timeObject.hour() * 60 * 60 + timeObject.minute() * 60 + timeObject.second()) * 1000) + timeObject.msec();
}
