#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractItemModel>
#include <QScopedPointer>
#include <QFileInfo>
#include <QUrl>
#include <QMediaPlaylist>

class PlaylistModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Column
    {
        Title = 0,
        ColumnCount
    };

    explicit PlaylistModel(QObject *subclass = nullptr);
    ~PlaylistModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QMediaPlaylist *playlist() const;
    void setPlaylist(QMediaPlaylist *playlist);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;

private slots:
    void letsStartInsertItems(int start, int end);
    void letsEndInsertItems();
    void letsStartRemoveItems(int start, int end);
    void letsEndRemoveItems();
    void letsChangeItems(int start, int end);

private:
    QScopedPointer<QMediaPlaylist> playList;
    QMap<QModelIndex, QVariant> mediaData;
};

#endif // PLAYLISTMODEL_H
