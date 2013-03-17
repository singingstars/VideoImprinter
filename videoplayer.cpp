#include "videoplayer.h"

#include <QtWidgets>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , mediaPlayer(0, QMediaPlayer::VideoSurface)
    , playButton(0)
    , positionSlider(0)
    , errorLabel(0)
{
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

    errorLabel = new QLabel(this);
    errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    // setup event text labels with video output in between
    QBoxLayout *labeledVideoLayout = new QHBoxLayout;
    QBoxLayout *leftLabelLayout = new QVBoxLayout;
    QBoxLayout *rightLabelLayout = new QVBoxLayout;
    for (int i=0; i<numOfLabels; i++)
    {
        eventLabel[i] = new QLabel(this);
        eventLabel[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
        highlightEventText(i, false);
        if (i % 2)
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

    eventLabel[iLabel]->setText(labelText);
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
