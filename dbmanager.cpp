#include "dbmanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <QFile>
#include <QFileInfo>

DbManager::DbManager(const QString& path)
{
   m_db = QSqlDatabase::addDatabase("QSQLITE");
   m_db.setDatabaseName(path);

   if (!m_db.open())
   {
      qDebug() << "Error: connection with database fail";
   }
   else
   {
      QSqlQuery query;
      query.prepare( "CREATE TABLE IF NOT EXISTS \"Track\" ( \"id\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, \"name\"	TEXT NOT NULL, \"artist_id\"	INTEGER,	\"album_id\"	INTEGER,	\"genre_id\"	INTEGER,	\"path\"	TEXT NOT NULL,	\"time\"	TEXT NOT NULL,	\"playcount\"	INTEGER NOT NULL DEFAULT 0,	\"rating\"	INTEGER NOT NULL DEFAULT 0 CHECK(rating<=5),	FOREIGN KEY(\"artist_id\") REFERENCES \"Artists\"(\"id\") ON DELETE SET NULL)" );
      if ( query.exec() )
      {

      }
      else
      {
        qDebug() << "Error creating Track Table!";
      }
      query.prepare( "CREATE TABLE IF NOT EXISTS \"Albums\" ( \"id\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, \"name\"	TEXT NOT NULL, \"artist_id\"	INTEGER NOT NULL, FOREIGN KEY(\"artist_id\") REFERENCES \"Artists\" )" );
      if ( query.exec() )
      {

      }
      else
      {
        qDebug() << "Error creating Album Table!";
      }
      query.prepare( "CREATE TABLE IF NOT EXISTS \"Artists\" ( \"id\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,	\"name\"	TEXT NOT NULL )" );
      if ( query.exec() )
      {

      }
      else
      {
        qDebug() << "Error creating Artists Table!";
      }
      query.prepare( "CREATE TABLE IF NOT EXISTS \"Genres\" (\"id\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, \"name\"	TEXT NOT NULL UNIQUE )" );
      if ( query.exec() )
      {

      }
      else
      {
        qDebug() << "Error creating Genres Table!";
      }
   }
}

int DbManager::addTrack( QString name, QString time, QString trackPath, int artistID, int albumID, int genreID )
{
    if ( ifExists( PATH, trackPath) )
        return 0;

    QSqlQuery query;
    query.prepare( "INSERT INTO Track ( name, artist_id, album_id, genre_id, path, time, playcount, rating ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ? )");
    query.addBindValue(name);
    query.addBindValue(artistID);
    query.addBindValue(albumID);
    query.addBindValue(genreID);
    query.addBindValue(trackPath);
    query.addBindValue(time);
    query.addBindValue(0);
    query.addBindValue(0);

    if ( !query.exec())
    {
        qDebug() << query.lastError().text();
        return 0;
    }

    return 1;
}

int DbManager::addArtist(QString artistName)
{
    if ( ifExists( ARTIST, artistName ) )
            return 0;

    QSqlQuery query;
    query.prepare( "INSERT INTO Artists ( name ) VALUES ( ?) ");
    query.addBindValue(artistName);

    if ( !query.exec())
        return 0;

    return 1;
}

int DbManager::addGenre(QString genreName)
{
    if ( ifExists( GENRE, genreName ) )
            return 0;

    QSqlQuery query;
    query.prepare( "INSERT INTO Genres ( name ) VALUES ( ?) ");
    query.addBindValue(genreName);

    if ( !query.exec())
        return 0;

    return 1;
}

int DbManager::addAlbum(QString albumName, int artist_id)
{
    if ( ifExists( ALBUM, albumName ) )
            return 0;

    QSqlQuery query;
    query.prepare( "INSERT INTO Albums ( name, artist_id ) VALUES ( ?, ? ) ");
    query.addBindValue(albumName);
    query.addBindValue(artist_id);

    if ( !query.exec())
        return 0;

    return 1;
}

int DbManager::getID(TAG type, QString value)
{
    if ( !ifExists( type, value ) )
            return -1;

    QSqlQuery query;
    switch ( type )
    {
        case PATH :
            query.prepare( "SELECT id FROM Track WHERE path = (?)" );
            query.addBindValue(value);
            break;
        case ARTIST:
            query.prepare( "SELECT id FROM Artists WHERE name = (?)" );
            query.addBindValue(value);
            break;
        case ALBUM :
            query.prepare( "SELECT id FROM Albums WHERE name = (?)" );
            query.addBindValue(value);
            break;
        case GENRE :
            query.prepare( "SELECT id FROM Genres WHERE name = (?)" );
            query.addBindValue(value);
            break;
    }

    if ( !query.exec() )
        qDebug() << "Error in query!" << query.lastError().text();

    if ( query.next() )
        return query.value(0).toInt();

    return 0;
}

int DbManager::getTrackid(QString trackPath)
{
    QSqlQuery query;
    query.prepare( "SELECT \"id\" FROM \"Track\" WHERE \"path\" = (:trackPath)");
    query.bindValue(":trackPath", trackPath );

    if ( query.exec() )
        return query.value(0).toInt();

    return 0;
}

QList<QStringList> * DbManager::getTracks()
{
    QList<QStringList> *tracks = new QList<QStringList>;
    QStringList *currentRecord;
    QSqlQuery query;
    query.prepare( "SELECT Track.name, time, Artists.name, Albums.name, Genres.name, playcount, rating, path FROM Track INNER JOIN Albums ON Albums.id = Track.album_id INNER JOIN Artists ON Artists.id = Track.artist_id INNER JOIN Genres ON Genres.id = Track.genre_id" );

    if ( !query.exec() )
         return new QList<QStringList>;

    while ( query.next() )
    {
        currentRecord = new QStringList;
        for ( int i = 0; i < 8; i++ )
        {
            currentRecord->append( query.value(i).toString() );
        }
        tracks->append(*currentRecord);
        delete( currentRecord );
    }
    return tracks;
}

QStringList DbManager::getGenres()
{
    QSqlQuery query;
    static QStringList genres;
    query.prepare( "SELECT name FROM Genres" );

    if ( query.exec() )
    {
        while ( query.next() )
        {
            genres.append( query.value(0).toString() );
        }

        return genres;
    }

    return QStringList();
}

QStringList DbManager::getArtists()
{
    QSqlQuery query;
    static QStringList artists;
    query.prepare( "SELECT name FROM Artists" );

    if ( query.exec() )
    {
        while ( query.next() )
        {
            artists.append( query.value(0).toString() );
        }

        return artists;
    }

    return QStringList();
}

QStringList DbManager::getAlbums()
{
    QSqlQuery query;
    static QStringList albums;
    query.prepare( "SELECT name FROM Albums" );

    if ( query.exec() )
    {
        while ( query.next() )
        {
            albums.append( query.value(0).toString() );
        }

        return albums;
    }

    return QStringList();
}

bool DbManager::ifExists( TAG type, QString value )
{
    QSqlQuery query;
    switch ( type )
    {
        case PATH :
            query.prepare( "SELECT id FROM Track WHERE path = (?)" );
            query.addBindValue(value);
            break;
        case ARTIST:
            query.prepare( "SELECT id FROM Artists WHERE name = (?)" );
            query.addBindValue(value);
            break;
        case ALBUM :
            query.prepare( "SELECT id FROM Albums WHERE name = (?)" );
            query.addBindValue(value);
            break;
        case GENRE :
            query.prepare( "SELECT id FROM Genres WHERE name = (?)" );
            query.addBindValue(value);
            break;
    }

    if ( !query.exec() )
        qDebug() << "Error in query!" << query.lastError().text();


    int count = 0;
    while ( query.next() )
        count++;


    if ( count )
        return 1;
    else {
        return 0;
    }
}

void DbManager::saveChanges()
{
    this->m_db.commit();
}

QString DbManager::getTrackPath(QString filename)
{
    QSqlQuery query;
    static QString path;
    query.prepare( "SELECT path FROM Track WHERE name = (?)" );
    query.addBindValue(filename);
    if ( !query.exec() )
        return NULL;

    if ( query.first() )
    {
        path = query.value(0).toString();
        return path;
    }

    return NULL;
}
