#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QtSql/QSqlDatabase>
#include <QPair>
#include <QList>

typedef enum
{
    TAG_ARTIST,
    TAG_ALBUM,
    TAG_ALBUM_ARTIST,
    TAG_GENRE,
    TAG_TRACKNAME,
    TAG_TARCKNUM,
    TAG_TRACKCOUNT,
    TAG_DISCNUM,
    TAG_DISCCOUNT,
    TAG_COMPOSER,
    TAG_COMMENTS,
    TAG_YEAR,
    TAG_BITRATE,
    TAG_CODEC,
    TAG_PATH,
    TAG_ARTISTID,
    TAG_ALBUMID,
    TAG_ALBUMARTISTID,
    TAG_GENREID,
    TAG_TRACKID,
    TAG_COMPOSERID,
    TAG_CODECID
} TAG_TYPE;


typedef struct
{
    QString tablename;
    QString fieldname;
} TagAttributes;

static QList<TagAttributes> tagProperties = {
    { "Artist", "name" },
    { "Album", "name" },
    { "Track", "album_artist" },
    { "Genre", "name" },
    { "Track", "name" },
    { "Track", "tracknumber" },
    { "Album", "trackcount" },
    { "Track", "discnumber" },
    { "Album", "disccount" },
    { "Composer", "name" },
    { "Track", "comment" },
    { "Track", "year" },
    { "Track", "bitrate" },
    { "Track", "codec" },
    { "Track", "path" },
    { "Artist", "id" },
    { "Album", "id" },
    { "Album", "artistid" },
    { "Genre", "id" },
    { "Track", "id" },
    { "Composer", "id" },
    { "Codec", "id" }
};


class DbManager
{
public:
    DbManager(const QString& path);
    QList<QStringList> * query( QString sql );
    QList<QStringList> * pullLibrary();

    int addTrack(QString name, QString time, QString trackPath, int artistID, int albumID, int genreID );
    int removeTrack( QString trackPath );

    int addArtist( QString artistName );
    int removeArtist( QString artistName );

    int addGenre( QString genreName );
    int removeGenre( QString genreName );

    int addAlbum( QString albumName, int artist_id );
    int removeAlbum( QString albumName );

    int getID( TAG_TYPE type, QString value );
    int getTrackid( QString trackPath );

    QList<QStringList> * getTracks();
    bool ifExists( TAG_TYPE type, QString value );
    void saveChanges();
    QString getTrackPath( QString filename );


private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
