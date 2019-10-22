#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QtSql/QSqlDatabase>

typedef enum
{
    PATH,
    ARTIST,
    ALBUM,
    GENRE
} TAG;

class DbManager
{
public:
    DbManager(const QString& path);
    int addTrack(QString name, QString time, QString trackPath, int artistID, int albumID, int genreID );
    int removeTrack( QString trackPath );
    int addArtist( QString artistName );
    int removeArtist( QString artistName );
    int addGenre( QString genreName );
    int removeGenre( QString genreName );
    int addAlbum( QString albumName, int artist_id );
    int removeAlbum( QString albumName );
    int getID( TAG type, QString value );
    int getTrackid( QString trackPath );
    QList<QStringList> * getTracks();
    QStringList getGenres();
    QStringList getArtists();
    QStringList getAlbums();
    bool ifExists( TAG type, QString value );
    void saveChanges();
    QString getTrackPath( QString filename );

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
