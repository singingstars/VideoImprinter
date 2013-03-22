#include "videoplayer.h"

#include <QtWidgets>
#include <QStringBuilder>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , mediaPlayer(0, QMediaPlayer::VideoSurface)
    , playButton(0)
    , positionSlider(0)
    , errorLabel(0)
{
    // setup playback speeds
    qreal tmpSpeeds[numOfSpeeds] = {-2.0, -1.0, 0.1, 0.25, 0.5
                                 , 1.0, 2.0, 4.0, 8.0, 12.0};
    currentSpeedId = 5;
    for (int i=0; i<numOfSpeeds; i++)
        speeds[i] = tmpSpeeds[i];

    QVideoWidget *videoWidget = new QVideoWidget(this);
    videoWidget->setMinimumSize(320, 240);

    QAbstractButton *openButton = new QPushButton(tr("Open..."), this);
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));

    playButton = new QPushButton(this);
    playButton->setEnabled(false);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(playButton, SIGNAL(clicked()),
            this, SLOT(play()));

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setRange(0, 0);

    connect(positionSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(setPosition(int)));

    timeLabel = new QLabel(this);

    connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(updateTime()));
    connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(updateTime()));

    errorLabel = new QLabel(this);
    errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    // setup event text labels with video output in between
    QBoxLayout *labeledVideoLayout = new QHBoxLayout;
    QBoxLayout *leftLabelLayout = new QVBoxLayout;
    QBoxLayout *rightLabelLayout = new QVBoxLayout;
    for (int i=0; i<numOfLabels; i++)
    {
        eventLabel[i] = new QLabel(this);
        eventLabel[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        highlightEventText(i, false);
        if (i < numOfLabels/2)
            leftLabelLayout->addWidget(eventLabel[i]);
        else
            rightLabelLayout->addWidget(eventLabel[i]);
    }
    labeledVideoLayout->addLayout(leftLabelLayout);
    labeledVideoLayout->addWidget(videoWidget);
    labeledVideoLayout->addLayout(rightLabelLayout);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(positionSlider);
    controlLayout->addWidget(timeLabel);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(labeledVideoLayout);
    layout->addLayout(controlLayout);
    layout->addWidget(errorLabel);

    setLayout(layout);

    mediaPlayer.setVideoOutput(videoWidget);
    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(&mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));

}

VideoPlayer::~VideoPlayer()
{
}

int VideoPlayer::getPosition()
{
    return (int)(mediaPlayer.position());
}

void VideoPlayer::highlightEventText(int iLabel, bool isOn)
{
    if ((iLabel >= numOfLabels) | (iLabel < 0))
        return;

    if (isOn)
        eventLabel[iLabel]->setStyleSheet("QLabel {background-color : yellow}");
    else
        eventLabel[iLabel]->setStyleSheet("QLabel {background : none}");
}

void VideoPlayer::setEventLabel(int iLabel, QString labelText)
{
    if ((iLabel >= numOfLabels) | (iLabel < 0))
        return;

    QString numberedLabel = QString("[%1] ").arg(iLabel) % labelText;
    eventLabel[iLabel]->setText(numberedLabel);
}

void VideoPlayer::openFile()
{
    errorLabel->setText("");

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath());

    if (!fileName.isEmpty()) {
        mediaPlayer.setMedia(QUrl::fromLocalFile(fileName));
        playButton->setEnabled(true);
    }
}

void VideoPlayer::play()
{
    switch(mediaPlayer.state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer.pause();
        break;
    default:
        mediaPlayer.play();
        break;
    }
}

void VideoPlayer::updateTime()
{
    qint64 len = mediaPlayer.duration();
    qint64 pos = mediaPlayer.position();
    QString timeString;
    if (pos || len)
    {
        qint64 sec = pos/1000;
        qint64 min = sec/60;
        qint64 hour = min/60;
        qint64 msec = pos;

        QTime playTime(hour%60, min%60, sec%60, msec%1000);
        sec = len / 1000;
        min = sec / 60;
        hour = min / 60;
        msec = len;

        QTime stopTime(hour%60, min%60, sec%60, msec%1000);
        QString playTimeFormat = "m:ss.zzz";
        if (hour > 0)
            playTimeFormat = "h:mm:ss.zzz";

        QString stopTimeFormat = "m:ss";
        if (hour > 0)
            stopTimeFormat = "h:mm:ss";

        timeString = playTime.toString(playTimeFormat);
        if (len)
            timeString += " / " + stopTime.toString(stopTimeFormat);
    }
    timeLabel->setText(timeString);
}

void VideoPlayer::jumpTo(int increament)
{
    mediaPlayer.setPosition(mediaPlayer.position() + increament);
}

void VideoPlayer::speedUp()
{
//    if (mediaPlayer.state() != QMediaPlayer::PlayingState)
//        return;

    if ((currentSpeedId < numOfSpeeds - 1) && (currentSpeedId >= 0))
        currentSpeedId++;
    else
        currentSpeedId = numOfSpeeds - 1;

    mediaPlayer.setPlaybackRate(speeds[currentSpeedId]);
    errorLabel->setText(QString("Speed: %1").arg(speeds[currentSpeedId]));
}

void VideoPlayer::speedDown()
{
//    if (mediaPlayer.state() != QMediaPlayer::PlayingState)
//        return;

    if ((currentSpeedId > 0) && (currentSpeedId < numOfSpeeds))
        currentSpeedId--;
    else
        currentSpeedId = 0;

    mediaPlayer.setPlaybackRate(speeds[currentSpeedId]);
    errorLabel->setText(QString("Speed: %1").arg(speeds[currentSpeedId]));
}

void VideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayer::positionChanged(qint64 position)
{
    positionSlider->setValue(position);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    positionSlider->setRange(0, duration);
}

void VideoPlayer::setPosition(int position)
{
    mediaPlayer.setPosition(position);
}

void VideoPlayer::handleError()
{
    playButton->setEnabled(false);
    errorLabel->setText("Error: " + mediaPlayer.errorString());
}
