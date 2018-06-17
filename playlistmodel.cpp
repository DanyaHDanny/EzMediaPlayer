#include "playlistmodel.h"

#include <QFileInfo>
#include <QUrl>
#include <QMediaPlaylist>

PlaylistModel::PlaylistModel(QObject *subclass) : QAbstractItemModel(subclass){}

PlaylistModel::~PlaylistModel(){}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    if(playList && !parent.isValid())
        return playList->mediaCount();
    else
        return 0;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return ColumnCount;
    else
        return 0;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return playList && !parent.isValid()
            && row >= 0 && row < playList->mediaCount()
            && column >= 0 && column < ColumnCount
        ? createIndex(row, column)
        : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DisplayRole) {
        QVariant value = mediaData[index];
        if (!value.isValid() && index.column() == Title) {
            QUrl location = playList->media(index.row()).canonicalUrl();
            return QFileInfo(location.path()).fileName();
        }

        return value;
    }
    return QVariant();
}

QMediaPlaylist *PlaylistModel::playlist() const
{
    return playList.data();
}

void PlaylistModel::setPlaylist(QMediaPlaylist *playlist)
{
    if (playList) {
        disconnect(playList.data(), &QMediaPlaylist::mediaAboutToBeInserted, this, &PlaylistModel::letsStartInsertItems);
        disconnect(playList.data(), &QMediaPlaylist::mediaInserted, this, &PlaylistModel::letsEndInsertItems);
        disconnect(playList.data(), &QMediaPlaylist::mediaAboutToBeRemoved, this, &PlaylistModel::letsStartRemoveItems);
        disconnect(playList.data(), &QMediaPlaylist::mediaRemoved, this, &PlaylistModel::letsEndRemoveItems);
        disconnect(playList.data(), &QMediaPlaylist::mediaChanged, this, &PlaylistModel::letsChangeItems);
    }

    beginResetModel();
    playList.reset(playlist);

    if (playList) {
        connect(playList.data(), &QMediaPlaylist::mediaAboutToBeInserted, this, &PlaylistModel::letsStartInsertItems);
        connect(playList.data(), &QMediaPlaylist::mediaInserted, this, &PlaylistModel::letsEndInsertItems);
        connect(playList.data(), &QMediaPlaylist::mediaAboutToBeRemoved, this, &PlaylistModel::letsStartRemoveItems);
        connect(playList.data(), &QMediaPlaylist::mediaRemoved, this, &PlaylistModel::letsEndRemoveItems);
        connect(playList.data(), &QMediaPlaylist::mediaChanged, this, &PlaylistModel::letsChangeItems);
    }

    endResetModel();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    mediaData[index] = value;
    emit dataChanged(index, index);
    return true;
}

void PlaylistModel::letsStartInsertItems(int start, int end)
{
    mediaData.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::letsEndInsertItems()
{
    endInsertRows();
}

void PlaylistModel::letsStartRemoveItems(int start, int end)
{
    mediaData.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::letsEndRemoveItems()
{
    endInsertRows();
}

void PlaylistModel::letsChangeItems(int start, int end)
{
    mediaData.clear();
    emit dataChanged(index(start,0), index(end,ColumnCount));
}
