#include "buttonsandcontrol.h"

ButtondAndControl::ButtondAndControl(QWidget *subclass) : QWidget(subclass)
{
    stopButton = new QToolButton(this);
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setEnabled(false);
    connect(stopButton, &QAbstractButton::clicked, this, &ButtondAndControl::stop);

    playButton = new QToolButton(this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playButton, &QAbstractButton::clicked, this, &ButtondAndControl::playClicked);

    previousButton = new QToolButton(this);
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(previousButton, &QAbstractButton::clicked, this, &ButtondAndControl::previous);

    nextButton = new QToolButton(this);
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    connect(nextButton, &QAbstractButton::clicked, this, &ButtondAndControl::next);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    connect(volumeSlider, &QSlider::valueChanged, this, &ButtondAndControl::volumeSliderChange);

    muteButton = new QToolButton(this);
    muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    connect(muteButton, &QAbstractButton::clicked, this, &ButtondAndControl::muteClicked);

    QBoxLayout *l = new QHBoxLayout;
    l->setMargin(0);
    l->addWidget(playButton);
    l->addWidget(stopButton);
    l->addWidget(previousButton);
    l->addWidget(nextButton);
    l->addWidget(muteButton);
    l->addWidget(volumeSlider);
    setLayout(l);
}

QMediaPlayer::State ButtondAndControl::state() const
{
    return playerState;
}

void ButtondAndControl::setState(QMediaPlayer::State state)
{
    if (state != playerState) {
        playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            stopButton->setEnabled(false);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        case QMediaPlayer::PlayingState:
            stopButton->setEnabled(true);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        case QMediaPlayer::PausedState:
            stopButton->setEnabled(true);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        }
    }
}

int ButtondAndControl::volume() const
{
    qreal linearVolume =  QAudio::convertVolume(volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);

    return qRound(linearVolume * 100);
}

void ButtondAndControl::setVolume(int volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100),
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

bool ButtondAndControl::isMuted() const
{
    return playerMuted;
}

void ButtondAndControl::setMuted(bool muted)
{
    if (muted != playerMuted) {
        playerMuted = muted;

        muteButton->setIcon(style()->standardIcon(muted
                ? QStyle::SP_MediaVolumeMuted
                : QStyle::SP_MediaVolume));
    }
}

void ButtondAndControl::playClicked()
{
    switch (playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    }
}

void ButtondAndControl::muteClicked()
{
    emit changeMuting(!playerMuted);
}

void ButtondAndControl::volumeSliderChange()
{
    emit changeVolume(volume());
}
