#include "dbmanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
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
       QList<QStringList> *results = nullptr;
       if ( ( results = query( "CREATE TABLE \"Album\" ( "
              "\"id\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, "
              "\"name\"	TEXT NOT NULL, "
              "\"artist_id\"	INTEGER NOT NULL, "
              "\"discs\"	INTEGER DEFAULT 1, "
              "\"tracks\"	INTEGER, "
              "FOREIGN KEY(\"artist_id\") REFERENCES \"Artist\" "
              ")") ) == nullptr )
       {
           qDebug() << "Failed to create Album Table";
       }
       else {
           delete results;
           results = nullptr;
       }

       if ( ( results = query( "CREATE TABLE \"Artist\" ( \"id\" "
              "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, "
              "\"name\"	TEXT NOT NULL )") ) == nullptr )
       {
           qDebug() << "Failed to create Artist Table";
       }
       else {
           delete results;
           results = nullptr;
       }

       if ( ( results = query( "CREATE TABLE \"Compilation\" ( "
              "\"id\"	INTEGER, \"artist_id\"	INTEGER )" ) ) == nullptr )
       {
           qDebug() << "Failed to create Compilation Table";
       }
       else {
           delete results;
           results = nullptr;
       }

       if ( ( results = query( "CREATE TABLE \"Composer\" ( "
              "\"id\"	INTEGER NOT NULL UNIQUE, \"name\" "
              "TEXT NOT NULL UNIQUE )" ) ) == nullptr )
       {
           qDebug() << "Failed to create Composer Table";
       }
       else {
           delete results;
           results = nullptr;
       }

       if ( ( results = query( "CREATE TABLE \"Equalizer\" ( "
              "\"id\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, "
              "\"name\"	TEXT NOT NULL, \"data\"	BLOB )" ) ) == nullptr )
       {
           qDebug() << "Failed to create Equalizer Table";
       }
       else {
           delete results;
           results = nullptr;
       }

       if ( ( results = query( "CREATE TABLE \"Genre\" (\"id\"	"
              "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, \"name\"	"
              "TEXT NOT NULL UNIQUE )"  ) ) == nullptr )
       {
           qDebug() << "Failed to create Genre Table";
       }
       else {
           delete results;
           results = nullptr;
       }

       if ( ( results = query( "CREATE TABLE \"Grouping\" ( "
              "\"id\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, "
              "\"name\"	TEXT NOT NULL UNIQUE )" ) ) == nullptr )
       {
           qDebug() << "Failed to create Grouping Table";
       }
       else {
           delete results;
           results = nullptr;
       }

       if ( ( results = query(
                  "CREATE TABLE \"Track\" ( "
                      "\"id\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, "
                      "\"name\"	TEXT NOT NULL, "
                      "\"album_id\"	INTEGER, "
                      "\"compilation_id\"	INTEGER, "
                      "\"genre_id\"	INTEGER, "
                      "\"path\"	TEXT NOT NULL, "
                      "\"time\"	TEXT NOT NULL, "
                      "\"playcount\"	INTEGER NOT NULL DEFAULT 0, "
                      "\"rating\"	INTEGER NOT NULL DEFAULT 0 CHECK(rating<=5), "
                      "\"composer_id\"	INTEGER, "
                      "\"year\"	INTEGER, "
                      "\"track\"	INTEGER, "
                      "\"disc\"	INTEGER DEFAULT 1, "
                      "\"compilation\"	INTEGER DEFAULT 0 CHECK(compilation>=0 AND compilation<=1), "
                      "\"lyrics\"	TEXT, "
                      "\"start\"	TEXT DEFAULT '0:00', "
                      "\"stop\"	TEXT, "
                      "\"position\"	TEXT DEFAULT '0:00', "
                      "\"skip\"	INTEGER DEFAULT 0, "
                      "\"equalizer_id\"	INTEGER DEFAULT 0, "
                      "\"sort_name\"	TEXT, "
                      "\"sort_album\"	INTEGER, "
                      "\"sort_artist\"	INTEGER, "
                      "\"sort_album_artist\"	INTEGER, "
                      "\"sort_composer\"	INTEGER, "
                      "\"kind\"	INTEGER, "
                      "\"size\"	INTEGER, "
                      "\"bitrate\"	INTEGER, "
                      "\"profile\"	INTEGER, "
                      "\"channels\"	INTEGER, "
                      "\"volume\"	INTEGER, "
                      "\"purchased_by\"	TEXT, "
                      "\"purchased_date\"	INTEGER, "
                      "\"date_modified\"	INTEGER, "
                      "\"date_added\"	INTEGER, "
                      "\"copyright\"	TEXT, "
                      "FOREIGN KEY(\"album_id\") REFERENCES \"Artist\"(\"id\") ON DELETE SET NULL, "
                      "FOREIGN KEY(\"compilation_id\") REFERENCES \"Album\"(\"id\") ON DELETE SET NULL, "
                      "FOREIGN KEY(\"genre_id\") REFERENCES \"Genre\"(\"id\") ON DELETE SET NULL, "
                      "FOREIGN KEY(\"composer_id\") REFERENCES \"Composer\"(\"id\") ON DELETE SET NULL )"
                  ) ) == nullptr )
       {
           qDebug() << "Failed to create Track Table";
       }
       else {
           delete results;
           results = nullptr;
       }


   }
}

int DbManager::addTrack( QString name, QString time, QString trackPath,
                         int artistID, int albumID, int genreID )
{
    if ( ifExists( TAG_PATH, trackPath) )
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
    if ( ifExists( TAG_ARTIST, artistName ) )
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
    if ( ifExists( TAG_GENRE, genreName ) )
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
    if ( ifExists( TAG_ALBUM, albumName ) )
            return 0;

    QSqlQuery query;
    query.prepare( "INSERT INTO Albums ( name, artist_id ) VALUES ( ?, ? ) ");
    query.addBindValue(albumName);
    query.addBindValue(artist_id);

    if ( !query.exec())
        return 0;

    return 1;
}

int DbManager::getID(TAG_TYPE type, QString value)
{
    if ( !ifExists( type, value ) )
            return -1;

    QSqlQuery query;
    switch ( type )
    {
        case TAG_PATH :
            query.prepare( "SELECT id FROM Track WHERE path = (?)" );
            query.addBindValue(value);
            break;
        case TAG_ARTIST:
            query.prepare( "SELECT id FROM Artists WHERE name = (?)" );
            query.addBindValue(value);
            break;
        case TAG_ALBUM :
            query.prepare( "SELECT id FROM Albums WHERE name = (?)" );
            query.addBindValue(value);
            break;
        case TAG_GENRE :
            query.prepare( "SELECT id FROM Genres WHERE name = (?)" );
            query.addBindValue(value);
            break;
        default :
            return 0;
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
    query.prepare(  "SELECT "
                        "Track.name, "
                        "time, "
                        "Artists.name,"
                        "Albums.name, "
                        "Genres.name, "
                        "playcount, "
                        "rating, "
                        "path "
                    "FROM "
                        "Track "
                    "INNER JOIN "
                        "Albums "
                    "ON "
                        "Albums.id = Track.album_id "
                    "INNER JOIN "
                        "Artists "
                    "ON "
                        "Artists.id = Track.artist_id "
                    "INNER JOIN "
                        "Genres "
                    "ON "
                        "Genres.id = Track.genre_id "
                    );

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

bool DbManager::ifExists( TAG_TYPE type, QString value )
{
    QSqlQuery query;
    switch ( type )
    {
        case TAG_PATH :
            query.prepare( "SELECT id FROM Track WHERE path = (?)" );
            query.addBindValue(value);
            break;
        case TAG_ARTIST:
            query.prepare( "SELECT id FROM Artists WHERE name = (?)" );
            query.addBindValue(value);
            break;
        case TAG_ALBUM :
            query.prepare( "SELECT id FROM Albums WHERE name = (?)" );
            query.addBindValue(value);
            break;
        case TAG_GENRE :
            query.prepare( "SELECT id FROM Genres WHERE name = (?)" );
            query.addBindValue(value);
            break;
        default :
            return 0;
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
    {
        qDebug() << query.lastError();
        return QString();
    }

    if ( query.first() )
    {
        path = query.value(0).toString();
        return path;
    }

    return QString();
}

QList<QStringList> * DbManager::query( QString sql )
{
    QSqlQuery query;
    QStringList *record = nullptr;
    QList<QStringList> *results = nullptr;

    query.prepare( sql );

    if ( !query.exec() )
    {
        qDebug() << query.lastError();
        return nullptr;
    }

    results = new QList<QStringList>();

    while ( query.next() )
    {
        record = new QStringList();

        for ( int i = 0; i < query.record().count();i++)
            record->append( query.value(i).toString() );

        results->append( QStringList( *record ) );

        for ( int i = 0; i < record->size(); i++ )
            qDebug() << record[i];

        delete record;
    }
    return results;

}

QList<QStringList> *DbManager::pullLibrary()
{
    QSqlQuery query;
    QStringList *record = nullptr;
    QList<QStringList> *results = nullptr;

    query.prepare( "SELECT "
                   "Track.id, "
                   "Track.disc AS \"Disc\", "
                   "Album.discs AS \"Disc Total\", "
                   "Track.track AS \"Track #\", "
                   "Album.tracks AS \"Total Tracks\", "
                   "Track.name, "
                   "Track.time AS \"Time\", "
                   "Album.name AS \"Album\", "
                   "Artist.name AS \"Album Artist\", "
                   "Comp.Artist, "
                   "Genre.name AS \"Genre\", "
                   "Track.playcount AS \"Playcount\", "
                   "Track.rating AS \"Rating\", "
                   "Composer.name AS \"Composer\", "
                   "Track.year AS \"Year\", "
                   "Track.compilation AS \"Compilation\", "
                   "Track.lyrics AS \"Lyrics\", "
                   "Track.start AS \"Start\", "
                   "Track.position AS \"Position\", "
                   "Track.stop AS \"Stop\", "
                   "Track.skip AS \"Skip\", "
                   "Track.sort_name AS \"Sort Name\", "
                   "SortAlbum.name AS \"Sort Album\", "
                   "SortArtist.name AS \"Sort Artist\", "
                   "SortAlbumArtist.name AS \"Sort Album Artist\", "
                   "Track.kind AS \"Kind\", "
                   "Track.size AS \"Size\", "
                   "Track.bitrate AS \"Bitrate\", "
                   "Track.profile AS \"Profile\", "
                   "Track.channels AS \"Channels\", "
                   "Track.volume AS \"Volume\", "
                   "Track.purchased_by AS \"Purchased By\", "
                   "Track.purchased_date AS \"Purchased Date\", "
                   "Track.date_modified AS \"Date Modified\", "
                   "Track.date_added AS \"Date Added\", "
                   "Track.copyright AS \"Copyright\" "
               "FROM "
                   "Track "
               "LEFT JOIN "
                   "Album "
               "ON "
                   "Album.id = Track.album_id "
               "LEFT JOIN "
                   "Artist "
               "ON "
                   "Album.artist_id = Artist.id "
               "LEFT JOIN "
                   "(SELECT "
                       "Compilation.id AS \"comp_id\", "
                       "group_concat( Artist.name, \" & \" ) AS \"Artist\" "
                   "FROM "
                       "Compilation "
                   "LEFT JOIN "
                       "Artist "
                   "ON "
                       "Compilation.artist_id = Artist.id) as \"Comp\" "
               "ON "
                       "Comp.comp_id = Track.compilation_id "
               "LEFT JOIN "
                   "Genre "
               "ON "
                   "Genre.id = Track.genre_id "
               "LEFT JOIN "
                   "Composer "
               "ON "
                   "Composer.id = Track.composer_id "
               "LEFT JOIN "
                   "(SELECT "
                       "Album.id AS \"id\", "
                       "Album.name AS \"name\" "
                   "FROM "
                       "Album "
                   "LEFT JOIN "
                       "Track "
                   "ON "
                       "Track.sort_album = album_id "
                   ") AS \"SortAlbum\" "
               "ON "
                   "SortAlbum.id = Track.sort_album "
               "LEFT JOIN "
                   "(SELECT "
                       "Compilation.id AS \"id\", "
                       "group_concat( Artist.name, \" & \" ) AS \"name\" "
                   "FROM "
                       "Compilation "
                   "LEFT JOIN "
                       "Artist "
                   "ON "
                       "Compilation.artist_id = Artist.id) as \"SortArtist\" "
               "ON "
                       "SortArtist.id = Track.sort_artist "
               "LEFT JOIN "
                   "Artist AS \"SortAlbumArtist\" "
               "ON "
                   "SortAlbumArtist.id = Track.sort_album_artist "
               "LEFT JOIN "
                   "Composer AS \"SortComposer\" "
               "ON "
                   "SortComposer.id = Track.sort_composer"
                );
    if ( !query.exec() )
    {
        qDebug() << query.lastError();
        return nullptr;
    }

    results = new QList<QStringList>();

    while ( query.next() )
    {
        record = new QStringList();

        for ( int i = 0; i < query.record().count();i++)
            record->append( query.value(i).toString() );

        results->append( QStringList( *record ) );

        for ( int i = 0; i < record->size(); i++ )
            qDebug() << record[i];

        delete record;
    }
    return results;
}
