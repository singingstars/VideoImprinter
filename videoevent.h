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

    VideoEvent(QObject *parent = 0);
    VideoEvent(int startTime, int endTime, QString eventText, QObject *parent = 0);
    ~VideoEvent();

    // data field id, must begin with 0
    enum fields {StartTimeField = 0
                 , EndTimeField = 1
                 , IntervalField = 2
                 , EventTextField = 3
                 , numOfFields = 4};

    int getStartTime(void) const;
    int getEndTime(void) const;
    int getInterval(void) const;
    QTime getStartQTime(void) const;
    QTime getEndQTime(void) const;
    QTime getQInterval(void) const;
    QString getEventText(void) const;

    void setStartTime(int time);
    void setStartTime(QTime time);
    void setEndTime(int time);
    void setEndTime(QTime time);
    void setEventText(QString text);

    // for sorting
    bool operator<(const VideoEvent & other);

    // for detecting duplicate
    bool operator==(const VideoEvent & other);

    static QTime QTimeFromMs(int totalmsecs);
    static QTime QTimeFromMs(qint64 totalmsecs);
    static int msFromQTime(QTime timeObject);

private:
    // Time is stored as mili-seconds, int in Qt is 32bit
    int startTime;
    int endTime;

    QString eventText;

};

#endif // VIDEOEVENT_H
