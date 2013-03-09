#ifndef VIDEOEVENT_H
#define VIDEOEVENT_H

#include <QObject>
#include <QString>
#include <QTime>

/**
 * @brief The VideoEvent class represents one video event.
 *
 *  The event is very similar to subtitle. One event contains
 *  a start time, and end time, an interval (caculated), and event text.
 */
class VideoEvent : public QObject
{
    Q_OBJECT
public:
    VideoEvent(long startTime, long endTime, QString eventText, QObject *parent = 0);
    ~VideoEvent();

    long getStartTime(void) const;
    long getEndTime(void) const;
    long getInterval(void) const;
    QTime getStartQTime(void) const;
    QTime getEndQTime(void) const;
    QTime getQInterval(void) const;
    QString getEventText(void) const;

    void setStartTime(long time);
    void setEndTime(long time);
    void setStartQTime(QTime time);
    void setEndQTime(QTime time);
    void setEventText(QString text);

    // for sorting
    bool operator<(const VideoEvent & other);

private:
    // Time is stored as mili-seconds
    long startTime;
    long endTime;

    QString eventText;

    static QTime QTimeFromMs(long totalmsecs);
    static long msFromQTime(QTime timeObject);

};

#endif // VIDEOEVENT_H
