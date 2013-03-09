// ldiv, ldiv_t
#include <stdlib.h>

#include "videoevent.h"

VideoEvent::VideoEvent(long startTime, long endTime, QString eventText, QObject *parent) :
    QObject(parent)
{
    this->startTime = startTime;
    this->endTime = endTime;
    this->eventText = eventText;
}

VideoEvent::~VideoEvent()
{
}

long VideoEvent::getStartTime() const
{
    return startTime;
}

long VideoEvent::getEndTime() const
{
    return endTime;
}

QString VideoEvent::getEventText() const
{
    return eventText;
}

long VideoEvent::getInterval() const
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

void VideoEvent::setStartTime(long time)
{
    this->startTime = time;
}

void VideoEvent::setEndTime(long time)
{
    this->endTime = time;
}

void VideoEvent::setStartQTime(QTime time)
{
    this->startTime = VideoEvent::msFromQTime(time);
}

void VideoEvent::setEndQTime(QTime time)
{
    this->endTime = VideoEvent::msFromQTime(time);
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

/**
 * @brief VideoEvent::QTimeFromMs
 * @param totalmsecs
 * @return
 *  copied from ksubtitle project's csubtitles.cpp, author Tom Deblauwe
 *  needs stdlib.h
 */
QTime VideoEvent::QTimeFromMs(long totalmsecs)
{
    // needs stdlib.h
    ldiv_t divresult;
    divresult = ldiv (totalmsecs, 1000);
    long hms = divresult.quot; //contains hours,minutes and seconds
    int msecs = divresult.rem;

    divresult = ldiv (hms, 60);
    long hm = divresult.quot; //contains hours and minutes
    int secs = divresult.rem;

    divresult = ldiv (hm, 60);
    long h = divresult.quot; //contains hours
    int mins = divresult.rem;

    int hours = (int) h;

    return QTime(hours, mins, secs, msecs);
}

/**
 * @brief VideoEvent::msFromQTime
 * @param timeObject
 * @return
 *
 *  copied from ksubtitle project's csubtitles.cpp, author Tom Deblauwe
 */
long VideoEvent::msFromQTime(QTime timeObject)
{
    return ((timeObject.hour() * 60 * 60 + timeObject.minute() * 60 + timeObject.second()) * 1000) + timeObject.msec();
}
