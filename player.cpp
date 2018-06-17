#include "player.h"

Player::Player(QWidget *subclass) : QWidget(subclass)
{
    QPalette qpalette = palette();
    qpalette.setColor(QPalette::Window, Qt::blue);
    this->setPalette(qpalette);
    this->setFixedSize(1000,500);

    player = new QMediaPlayer(this);
    player->setAudioRole(QAudio::VideoRole);
    playlist = new QMediaPlaylist();
    player->setPlaylist(playlist);

    connect(player, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &Player::positionChanged);
    connect(player, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &Player::metaDataChanged);
    connect(playlist, &QMediaPlaylist::currentIndexChanged, this, &Player::playlistPositionChanged);
    connect(player, &QMediaPlayer::bufferStatusChanged, this, &Player::bufferingProgress);
    connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Player::displayErrorMessage);

    videoWidget = new Screen(this);
    player->setVideoOutput(videoWidget);

    playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(playlist);

    playlistView = new QListView(this);
    playlistView->setModel(playlistModel);
    playlistView->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));

    connect(playlistView, &QAbstractItemView::activated, this, &Player::jump);

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, player->duration() / 1000);

    labelDuration = new QLabel(this);
    connect(slider, &QSlider::sliderMoved, this, &Player::seek);

    QPushButton *openButton = new QPushButton(this);
    openButton ->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    connect(openButton, &QPushButton::clicked, this, &Player::open);

    ButtondAndControl *controls = new ButtondAndControl(this);
    controls->setState(player->state());
    controls->setVolume(player->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, &ButtondAndControl::play, player, &QMediaPlayer::play);
    connect(controls, &ButtondAndControl::pause, player, &QMediaPlayer::pause);
    connect(controls, &ButtondAndControl::stop, player, &QMediaPlayer::stop);
    connect(controls, &ButtondAndControl::next, playlist, &QMediaPlaylist::next);
    connect(controls, &ButtondAndControl::previous, this, &Player::previousClicked);
    connect(controls, &ButtondAndControl::changeVolume, player, &QMediaPlayer::setVolume);
    connect(controls, &ButtondAndControl::changeMuting, player, &QMediaPlayer::setMuted);
    connect(controls, &ButtondAndControl::stop, videoWidget, QOverload<>::of(&QVideoWidget::update));

    connect(player, &QMediaPlayer::stateChanged, controls, &ButtondAndControl::setState);
    connect(player, &QMediaPlayer::volumeChanged, controls, &ButtondAndControl::setVolume);
    connect(player, &QMediaPlayer::mutedChanged, controls, &ButtondAndControl::setMuted);

    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(playlistView);
    displayLayout->addWidget(videoWidget, 2);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(slider);
    hLayout->addWidget(labelDuration);
    layout->addLayout(hLayout);
    layout->addLayout(controlLayout);

    setLayout(layout);

    metaDataChanged();
}

void Player::open()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));
    QStringList supportedMimeTypes = player->supportedMimeTypes();
    if (!supportedMimeTypes.isEmpty()) {
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted)
        addToPlaylist(fileDialog.selectedUrls());
}

static bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void Player::addToPlaylist(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            playlist->load(url);
        else
            playlist->addMedia(url);
    }
}

void Player::durationChanged(qint64 d)
{
    duration = d / 1000;
    slider->setMaximum(duration);
}

void Player::positionChanged(qint64 p)
{
    if (!slider->isSliderDown())
        slider->setValue(p / 1000);

    upDateDurationInfo(p / 1000);
}

void Player::metaDataChanged()
{
    if (player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(player->metaData(QMediaMetaData::AlbumArtist).toString())
                .arg(player->metaData(QMediaMetaData::Title).toString()));

        if (coverLabel) {
            QUrl url = player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }
    }
}

void Player::previousClicked()
{
    if (player->position() <= 3000)
        playlist->previous();
    else
        player->setPosition(0);
}

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        playlist->setCurrentIndex(index.row());
        player->play();
    }
}

void Player::playlistPositionChanged(int currentItem)
{
    playlistView->setCurrentIndex(playlistModel->index(currentItem, 0));
}

void Player::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

void Player::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
}

void Player::setTrackInfo(const QString &info)
{
    trackInfo = info;
    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);
}

void Player::setStatusInfo(const QString &info)
{
    statusInfo = info;
    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);
}

void Player::displayErrorMessage()
{
    setStatusInfo(player->errorString());
}

void Player::upDateDurationInfo(qint64 currentInfo)
{
    QString qs;
    if (currentInfo || duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((duration / 3600) % 60, (duration / 60) % 60,
            duration % 60, (duration * 1000) % 1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        qs = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    labelDuration->setText(qs);
}

