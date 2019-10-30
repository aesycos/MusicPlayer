#include <QFile>
#include <QString>
#include <QJsonDocument>

#ifndef SETTINGS_H
#define SETTINGS_H


class Settings
{
public :
    Settings();
    Settings( const QFile *filepath );
    bool loadExternal();
    bool readSettings();
    void saveSettings();
    void close();
    bool parseSettings();
    void setFilePath( const QFile *filepath );
    QFile * getFilePath();

    int getVolume();
    void setVolume( int level );

    QString *getLibraryPath();
    void setLibraryPath( const QString path );

    QStringList *getWatchedFolders();
    void setWatchedFolders( const QStringList folders );

    QStringList *getPlaylists();
    void setPlaylists( const QStringList playlists );

    QString *getDbName();
    void setDbName( const QString dbname );

    QStringList *getDefaultColumns();
    void setDefaultColums( const QStringList columns );

    bool isInternal();
    bool isLoaded();

    bool isError();
    QString * lastError();

private :
    Settings *external = nullptr;
    bool internal = true;
    bool loaded = false;
    int volume = 0;
    QFile *filepath = nullptr;
    QFile *externalFilePath = nullptr;
    QString *libraryPath = nullptr;
    QString *dbName = nullptr;
    QStringList *defaultColumnList = nullptr;
    QStringList *playlists = nullptr;
    QStringList *watchedFoldersList = nullptr;
    QFile *playlistsPath = nullptr;
    QString *error = nullptr;
    QString *data = nullptr;
    QJsonDocument *json = nullptr;

    void setError( const QString errorMsg );

signals :
    void changedData( QVariant data );

};

#endif // SETTINGS_H
