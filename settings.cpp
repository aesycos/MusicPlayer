#include "settings.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QDebug>

Settings::Settings()
{

}

Settings::Settings( const QFile *filepath)
{
    setFilePath( filepath );

    if ( readSettings() )
        parseSettings();
}

bool Settings::loadExternal()
{
    if ( !(external == nullptr) )
    {
        external->saveSettings();
        external->close();
        delete external;
    }

    if ( ( externalFilePath == nullptr) ||
         (externalFilePath->fileName().isEmpty() ) ) {
        setError( QString("No external file path found.") );
        return false;
    }

    external = new Settings( externalFilePath );

    return true;
}

bool Settings::readSettings()
{
    if ( !filepath->isReadable() )
        setError( QString( "Settings file `%1` is not readable" ).arg( filepath->fileName() ) );

    if ( !filepath->open(QIODevice::ReadOnly ) )
    {
        setError( QString( "Failed to open file `%1`: %2").arg( filepath->fileName() ).arg( filepath->errorString() ) );
        filepath->unsetError();

        return false;
    }


    if ( data )
        delete data;

    data = new QString( filepath->read(2048) );

    if ( data->isEmpty() )
    {
        if ( filepath->error() )
        {
            setError( QString( "An error occured while reading config file: %1" ).arg( filepath->errorString() ) );
            filepath->unsetError();
            return false;
        }
        setError( QString( "Warning Config file read in 0 bytes of data") );
    }

    loaded = true;

    return true;
}

void Settings::saveSettings()
{
    if ( isLoaded() )
        filepath->write( QByteArray() );
}

void Settings::close()
{
    if ( filepath->isOpen() )
        filepath->close();
    this->loaded = false;
}

bool Settings::parseSettings()
{
    if ( !data )
    {
        setError( "Config file needs to be read befrore parsing");
        return false;
    }

    json =  new QJsonDocument( QJsonDocument::fromJson( data->toUtf8() ) );

    if ( json->isNull() )
    {
        setError( "Config file is invalid");
        return false;
    }

    QJsonObject jsonObj;

    jsonObj = json->object();

    QString key;

    key = "Library Path";
    if ( jsonObj.contains(key) && jsonObj[key].isString() )
        setLibraryPath( jsonObj[key].toString() );

    key = "Database";
    if ( jsonObj.contains(key) && jsonObj[key].isString() )
        setDbName( jsonObj[key].toString() );

    key = "Volume";
    if ( jsonObj.contains(key) && jsonObj[key].isDouble() )
        setVolume( int( jsonObj[key].toDouble() ) );

    key = "Default Columns";
    if ( jsonObj.contains(key) && jsonObj[key].isArray() )
        setDefaultColums( jsonObj[key].toVariant().toStringList() );

    key = "Watched Folders";
    if ( jsonObj.contains(key) && jsonObj[key].isArray() )
        setWatchedFolders( jsonObj[key].toVariant().toStringList() );
    return true;
}

void Settings::setFilePath( const QFile *filepath)
{
    if ( this->filepath )
        delete this->filepath;
    this->filepath = new QFile( filepath->fileName() );
}

QFile *Settings::getFilePath()
{
    return filepath;
}

int Settings::getVolume()
{
    return volume;
}

void Settings::setVolume(int level)
{
    volume = level;
}

QString *Settings::getLibraryPath()
{
    return libraryPath;
}

void Settings::setLibraryPath(const QString path)
{
    if ( libraryPath )
        delete libraryPath;
    libraryPath = new QString( path );
}

QStringList *Settings::getWatchedFolders()
{
    return watchedFoldersList;
}

void Settings::setWatchedFolders(const QStringList folders)
{
    if ( watchedFoldersList )
        delete watchedFoldersList;

    watchedFoldersList = new QStringList( folders );
}

QStringList *Settings::getPlaylists()
{
    return playlists;
}

void Settings::setPlaylists(const QStringList playlists)
{
    if ( this->playlists )
        delete this->playlists;
    this->playlists = new QStringList( playlists );
}

QString *Settings::getDbName()
{
    return dbName;
}

void Settings::setDbName(const QString dbname)
{
    if ( this->dbName )
        delete this->dbName;
    this->dbName = new QString( dbname );
}

QStringList *Settings::getDefaultColumns()
{
    return defaultColumnList;
}

void Settings::setDefaultColums(const QStringList columns)
{
    if ( defaultColumnList )
        delete defaultColumnList;
    defaultColumnList = new QStringList( columns );
}

bool Settings::isInternal()
{
    return internal;
}

bool Settings::isLoaded()
{
    if ( filepath->openMode() )
        return true;
    else {
        return false;
    }
}

bool Settings::isError()
{
    if ( !error || error->isEmpty() )
        return false;
    return true;
}

QString * Settings::lastError()
{
    if ( isError() )
        return error;
    else {
        return nullptr;
    }
}

void Settings::setError(const QString errorMsg)
{
    if ( error )
        delete error;

    error = new QString( errorMsg );
}
