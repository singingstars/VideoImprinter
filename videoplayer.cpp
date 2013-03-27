#include "videoplayer.h"

#include <QtWidgets>
#include <QStringBuilder>
#include <QMessageBox>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , playButton(0)
    , positionSlider(0)
    , errorLabel(0)
    , isFileSet(false)
{
    mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    mediaPlayer->setNotifyInterval(50);

    // setup playback speeds
    qreal tmpSpeeds[numOfSpeeds] = {-2.0, -1.0, 0.1, 0.25, 0.5
                                 , 1.0, 2.0, 4.0, 8.0, 12.0};
    currentSpeedId = 5;
    for (int i=0; i<numOfSpeeds; i++)
        speeds[i] = tmpSpeeds[i];

    videoWidget = new QVideoWidget(this);
    videoWidget->setMinimumSize(320, 240);
    videoWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

//    QAbstractButton *openButton = new QPushButton(tr("Open..."), this);
//    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));

    playButton = new QPushButton(this);
    playButton->setEnabled(false);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(this, SIGNAL(playToggled()), playButton, SIGNAL(clicked()));
    connect(playButton, SIGNAL(clicked()),
            this, SLOT(play()));

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setRange(0, 0);

    connect(positionSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(setPosition(int)));

    timeLabel = new QLabel(this);

    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(updateTime()));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(updateTime()));

    volumnSlider = new QSlider(Qt::Vertical, this);
    volumnSlider->setMaximumSize(20, 35);
    volumnSlider->setRange(0, 100);
    mediaPlayer->setVolume(0);
    volumnSlider->setValue(mediaPlayer->volume());

    connect(mediaPlayer, SIGNAL(volumeChanged(int)), volumnSlider, SLOT(setValue(int)));
    connect(volumnSlider, SIGNAL(sliderMoved(int)), mediaPlayer, SLOT(setVolume(int)));

    errorLabel = new QLabel(this);
    errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    // setup event text labels with video output in between
    QBoxLayout *labeledVideoLayout = new QHBoxLayout;
    QBoxLayout *leftLabelLayout = new QVBoxLayout;
    QBoxLayout *rightLabelLayout = new QVBoxLayout;
    for (int i=0; i<numOfLabels; i++)
    {
        eventLabel[i] = new QLabel(this);
        eventLabel[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        highlightEventText(i, false);
//        if (i < numOfLabels/2)
            leftLabelLayout->addWidget(eventLabel[i]);
//        else
//            rightLabelLayout->addWidget(eventLabel[i]);
    }
    labeledVideoLayout->addLayout(leftLabelLayout);
    labeledVideoLayout->addWidget(videoWidget);
    labeledVideoLayout->addLayout(rightLabelLayout);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
//    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(positionSlider);
    controlLayout->addWidget(timeLabel);
    controlLayout->addWidget(volumnSlider);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(labeledVideoLayout);
    layout->addLayout(controlLayout);
    layout->addWidget(errorLabel);

    setLayout(layout);

    mediaPlayer->setVideoOutput(videoWidget);
    connect(mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SIGNAL(playerPositionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));

}

VideoPlayer::~VideoPlayer()
{
}

int VideoPlayer::getPosition()
{
    return (int)(mediaPlayer->position());
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

void VideoPlayer::setHasMediaFile(bool f)
{
    isFileSet = f;
    emit mediaFileStatusChanged(isFileSet);
}

bool VideoPlayer::hasMediaFile()
{
    return isFileSet;
}

int VideoPlayer::getBrightness()
{
    return videoWidget->brightness();
}

int VideoPlayer::getContrast()
{
    return videoWidget->contrast();
}

int VideoPlayer::getHue()
{
    return videoWidget->hue();
}

int VideoPlayer::getSaturation()
{
    return videoWidget->saturation();
}

void VideoPlayer::openFile()
{
    if(hasMediaFile())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Video Player"),
                     tr("Already has a media file.\n"
                        "Open a new one?"),
                     QMessageBox::Open | QMessageBox::Cancel);

        if (ret == QMessageBox::Cancel)
            return;
    }

     QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie")
                                                , QDir::homePath()
                                                , tr("Any Media (*.*)"));

    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

void VideoPlayer::loadFile(const QString fileName)
{
    if (QFile::exists(fileName)) {
        mediaPlayer->setMedia(QUrl::fromLocalFile(fileName));
        playButton->setEnabled(true);
        setHasMediaFile(true);
    }
}

void VideoPlayer::play()
{
    switch(mediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer->pause();
        break;
    default:
        mediaPlayer->play();
        break;
    }
}

void VideoPlayer::updateTime()
{
    qint64 len = mediaPlayer->duration();
    qint64 pos = mediaPlayer->position();
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
    mediaPlayer->setPosition(mediaPlayer->position() + increament);

}

void VideoPlayer::speedUp()
{
//    if (mediaPlayer->state() != QMediaPlayer::PlayingState)
//        return;

    if ((currentSpeedId < numOfSpeeds - 1) && (currentSpeedId >= 0))
        currentSpeedId++;
    else
        currentSpeedId = numOfSpeeds - 1;

    mediaPlayer->setPlaybackRate(speeds[currentSpeedId]);
    errorLabel->setText(QString("Speed: %1").arg(speeds[currentSpeedId]));
}

void VideoPlayer::speedDown()
{
//    if (mediaPlayer->state() != QMediaPlayer::PlayingState)
//        return;

    if ((currentSpeedId > 0) && (currentSpeedId < numOfSpeeds))
        currentSpeedId--;
    else
        currentSpeedId = 0;

    mediaPlayer->setPlaybackRate(speeds[currentSpeedId]);
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
    mediaPlayer->setPosition(position);
}

void VideoPlayer::setBrightness(int brightness)
{
    videoWidget->setBrightness(brightness);
    emit brightnessChanged(brightness);
}

void VideoPlayer::setContrast(int contrast)
{
    videoWidget->setContrast(contrast);
    emit contrastChanged(contrast);
}
void VideoPlayer::setHue(int hue)
{
    videoWidget->setContrast(hue);
    emit contrastChanged(hue);
}

void VideoPlayer::setSaturation(int saturation)
{
    videoWidget->setSaturation(saturation);
    emit saturationChanged(saturation);
}

void VideoPlayer::handleError()
{
    playButton->setEnabled(false);
    errorLabel->setText("Error: " + mediaPlayer->errorString());
}
