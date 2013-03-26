#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <qmediaplayer.h>

#include <QtGui/QMovie>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QSlider;
class QLabel;
QT_END_NAMESPACE

/**
 * @brief The VideoPlayer widget handles video playback.
 * It was largely originated from Qt5.0 example "videowidget".
 */
class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

    int getPosition();
    void highlightEventText(int iLabel, bool isOn);
    void setEventLabel(int iLabel, QString labelText);
    void setHasMediaFile(bool f = true);
    bool hasMediaFile();

    int getBrightness();
    int getContrast();
    int getHue();
    int getSaturation();

public slots:
    void openFile();
    void loadFile(const QString fileName);
    void play();
    void updateTime();
    void jumpTo(int increament);
    void speedUp();
    void speedDown();

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
    void setBrightness(int brightness);
    void setContrast(int contrast);
    void setHue(int hue);
    void setSaturation(int saturation);
    void handleError();

signals:
    void mediaFileStatusChanged(bool);
    void playToggled();
    void brightnessChanged(int);
    void contrastChanged(int);
    void hueChanged(int);
    void saturationChanged(int);

private:
    enum {numOfLabels = 10, numOfSpeeds = 10};

    QMediaPlayer mediaPlayer;
    QVideoWidget *videoWidget;
    QAbstractButton *playButton;
    QSlider *positionSlider;
    QSlider *volumnSlider;
    QLabel *timeLabel;
    QLabel *errorLabel;

    // a batch of labels indicating on/off of video-events
    QLabel *eventLabel[numOfLabels];

    // playback speed levels
    int currentSpeedId;
    qreal speeds[numOfSpeeds];

    bool isFileSet;

    void setupEventTextLabels();
};

#endif // VIDEOPLAYER_H
