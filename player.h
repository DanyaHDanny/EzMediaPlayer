#ifndef PLAYER_H
#define PLAYER_H

#include "buttonsandcontrol.h"
#include "playlistmodel.h"
#include "screen.h"

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QAbstractItemView>
#include <QLabel>
#include <QMediaPlayer>
#include <QModelIndex>
#include <QPushButton>
#include <QSlider>
#include <QVideoProbe>
#include <QVideoWidget>
#include <QAudioProbe>
#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QtWidgets>

class PlaylistModel;

class Player : public QWidget
{
    Q_OBJECT

public:
    Player(QWidget *subclass = nullptr);

    void addToPlaylist(const QList<QUrl> &urls);

signals:

private slots:
    void open();
    void durationChanged(qint64 d);
    void positionChanged(qint64 p);
    void metaDataChanged();

    void previousClicked();

    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);

    void bufferingProgress(int progress);

    void displayErrorMessage();


private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void upDateDurationInfo(qint64 currentInfo);

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videoWidget;
    QLabel *coverLabel;
    QSlider *slider;
    QLabel *labelDuration;

    PlaylistModel *playlistModel;
    QAbstractItemView *playlistView;
    QString trackInfo;
    QString statusInfo;
    qint64 duration;
};

#endif // PLAYER_H
