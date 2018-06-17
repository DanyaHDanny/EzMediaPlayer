#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QMediaPlayer>
#include <QWidget>
#include <QAbstractButton>
#include <QAbstractSlider>
#include <QComboBox>
#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>
#include <QAudio>


class ButtondAndControl : public QWidget
{
    Q_OBJECT

public:
    explicit ButtondAndControl(QWidget *subclass = nullptr);

    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void changeMuting(bool muting);

private slots:
    void playClicked();
    void muteClicked();
    void volumeSliderChange();

private:
    QMediaPlayer::State playerState = QMediaPlayer::StoppedState;
    QAbstractButton *playButton;
    QAbstractButton *stopButton;
    QAbstractButton *nextButton;
    QAbstractButton *previousButton;
    QAbstractButton *muteButton;
    QAbstractSlider *volumeSlider;
    bool playerMuted = false;
};

#endif // PLAYERCONTROLS_H
