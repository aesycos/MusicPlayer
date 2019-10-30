#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

// Media Playback
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QMediaResource>

// For displaying artwork
#include <QPixmap>
#include <QGraphicsPixmapItem>

#include <QStack>
#include <QColumnView>
#include <QStandardItemModel>
#include <QStandardItem>

// Folder and File Selections
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>

// Handling the library database
#include <QtSql/QSql>

// Reading metadata from files for database
#include <taglib/tag.h>
#include <taglib/fileref.h>

#include "dbmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui->setupUi(this);


    // Read Config File
    readConfig();

    ui->watchFoldersListView->setModel( watchedFolders );

    // Setup TableView
    ui->libraryTable->setColumnCount( defaultColumns->size() );
    ui->libraryTable->setHorizontalHeaderLabels( *defaultColumns );

    // Get Music Library Info
    initDB();

    player = new QMediaPlayer;

    //for ( int i = 0; i < ui->libraryTable->horizontalHeader()->count(); i++ )
    //{
    //    qDebug() << i << ": " << ui->libraryTable->horizontalHeaderItem( i )->text();
    //}

    updateLibrary();
    ui->libraryTable->resizeColumnsToContents();
    ui->libraryTable->resizeRowsToContents();
    ui->progressBar->setMinimum(0);

    // Setup UI control slot/signals
    {
    connect( ui->volumeSlider, SIGNAL( valueChanged(int)), player, SLOT(setVolume(int)) );
    connect( player, SIGNAL( volumeChanged(int)), ui->volumeSlider, SLOT(setValue(int)) );
    connect( player, SIGNAL( durationChanged( qint64 ) ), this, SLOT( durationChanged( qint64 ) ) );
    connect( player, SIGNAL( positionChanged( qint64 ) ), this, SLOT( positionChanged( qint64 ) ) );
    connect( player, SIGNAL( stateChanged( QMediaPlayer::State ) ), this, SLOT( playButtonState( QMediaPlayer::State ) ) );
    connect( player, SIGNAL( metaDataAvailableChanged(bool) ), this, SLOT( updateTrackInfo(bool) ) );
    }

    player->setVolume( volume );
    ui->contentPanel->setCurrentWidget( ui->libraryPage );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_searchButton_clicked()
{
    ui->contentPanel->setCurrentWidget(ui->searchPage);
}

void MainWindow::on_dashboardButton_clicked()
{
    ui->contentPanel->setCurrentWidget(ui->dashboardPage);
}

void MainWindow::on_volumeSlider_valueChanged(int value) {
    QPushButton *volume = ui->muteButton;
    volume->setIcon( volumeIcons[value/33] );
}

void MainWindow::on_libraryButton_clicked() {
   ui->contentPanel->setCurrentWidget(ui->libraryPage);
}

void MainWindow::on_closeButton_clicked() {
    close();
}

void MainWindow::positionChanged( qint64 position ) {
    ui->progressBar->setValue( int( position / 1000 ) );
}

void MainWindow::durationChanged( qint64 position ) {
    ui->progressBar->setMaximum( int( position / 1000 ) );
}

void MainWindow::on_playButton_clicked() {
    if ( player->state() == 1 )
        player->play();
    else
        player->pause();
}

void MainWindow::playButtonState(QMediaPlayer::State state) {
     ui->playButton->setIcon( playButtonIcons[state] );
}

void MainWindow::updateTrackInfo(bool available)
{
    if (!available)
        return;

    QString trackTitle = player->metaData(QMediaMetaData::Title).toString();
    QString artist = player->metaData(QMediaMetaData::AlbumArtist).toString();
    QString album = player->metaData(QMediaMetaData::AlbumTitle).toString();
    QImage cover = player->metaData(QMediaMetaData::CoverArtImage).value<QImage>();

    ui->trackTitle->setText(trackTitle);
    ui->albumLabel->setText(album);
    ui->artistLabel->setText(artist);
    if ( !cover.isNull() )
    {

        ui->albumArtThumb->setPixmap( QPixmap::fromImage( cover ) );
    }
    else {
        ui->albumArtThumb->setPixmap( QPixmap( ":/icons/icons/stolencover.png" ) );
    }
}

void MainWindow::on_libraryTable_itemDoubleClicked(QTableWidgetItem *item)
{
    QString filepath;
    filepath = ui->libraryTable->item(item->row(), 7)->text();
    if ( !filepath.isNull() )
    {
        player->setMedia( QMediaContent( QUrl::fromLocalFile(filepath) ) );
        player->play();
    }
}

void MainWindow::on_settingsButton_clicked()
{
   ui->contentPanel->setCurrentWidget(ui->settingsPage);
}

void MainWindow::initDB()
{

}

void MainWindow::importFolder(QDir *parentFolder)
{
    parentFolder->setFilter(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    QStringList qsl; qsl.append("*.mp3");
    parentFolder->setNameFilters(qsl);

    QStack<QString> *fileList = new QStack<QString>();

    // get a files recursively in parent directory
    QDirIterator it( *parentFolder, QDirIterator::Subdirectories);
    while ( it.hasNext() )
        fileList->push(it.next());

    QString currentFile;
    //MetaData * currentMeta;
    QString title, artist, album, genre, time;
    int seconds;
    int minutes;

    int genreID, artistID, albumID;

    int count = 0;
    while ( !fileList->isEmpty() )
    {
        currentFile = fileList->pop();

        if ( library_db->ifExists( PATH, currentFile ) )
            continue;

        TagLib::FileRef f( currentFile.toUtf8() );
        title = f.tag()->title().toCString();
        artist = f.tag()->artist().toCString();
        album = f.tag()->album().toCString();
        genre = f.tag()->genre().toCString();


        seconds = f.audioProperties()->lengthInSeconds();
        minutes = seconds/60;
        seconds %= 60;
        time = QString( "%1:%2" ).arg( minutes, 2, 10, QChar( '0') ).arg( seconds, 2, 10, QChar('0') );

        library_db->addArtist( artist );
        artistID = library_db->getID( ARTIST, artist );

        library_db->addGenre( genre );
        genreID = library_db->getID( GENRE, genre );

        library_db->addAlbum( album, artistID );
        albumID = library_db->getID( ALBUM, album );


        library_db->addTrack( title, time, currentFile, artistID, albumID, genreID );
        count++;
    }
}

void MainWindow::addToWatchedFolders(QString folder)
{
    if ( watchedFolders->findItems( folder, Qt::MatchExactly ).size() == 0 )
        watchedFolders->appendRow( new QStandardItem( folder ) );

    emit watchedFoldersChanged( folder );
}

void MainWindow::addToWatchedFolders(QList<QUrl> folderList)
{
    while ( folderList.size() > 0 )
        addToWatchedFolders( folderList.takeAt(0).toLocalFile() );
}

bool MainWindow::isConfigLoaded()
{
    if ( settings == nullptr )
        return false;
    else {
        return true;
    }
}

void MainWindow::updateLibrary()
{
    QStringList qsl_genrelist = library_db->getGenres();
    QStandardItemModel *genreModel = new QStandardItemModel;
    for ( int i = 0; i < qsl_genrelist.size(); i++ )
    {
        genreModel->appendRow( new QStandardItem( qsl_genrelist.at(i)));
    }
    ui->genreList->setModel(genreModel);

    QStringList qsl_artistlist = library_db->getArtists();
    QStandardItemModel *artistModel = new QStandardItemModel;
    for ( int i = 0; i < qsl_artistlist.size(); i++ )
    {
        artistModel->appendRow( new QStandardItem( qsl_artistlist.at(i)));
    }
    ui->artistList->setModel(artistModel);

    QStringList qsl_albumlist = library_db->getAlbums();
    QStandardItemModel *albumModel = new QStandardItemModel;
    for ( int i = 0; i < qsl_albumlist.size(); i++ )
    {
        albumModel->appendRow( new QStandardItem( qsl_albumlist.at(i)));
    }
    ui->albumList->setModel(albumModel);

    QList<QStringList> *tracks;
    tracks = library_db->getTracks();

    for ( int row = 0; row < tracks->size(); row++ )
    {
        ui->libraryTable->insertRow( ui->libraryTable->rowCount() );

        for ( int col = 0; col < 8; col++)
            ui->libraryTable->setItem( ui->libraryTable->rowCount()-1, col, new QTableWidgetItem( QString( tracks->at(row).at(col) ) ) );

    }
}

bool MainWindow::readConfig()
{
    settings = new Settings( new QFile() );

    // TODO Handle internal vs external config loading;
    // Internal config should always exist as fallback if external config is
    // available should point to and load it
    /*if ( settings->isInternal() )
        settings->isError();
    else {
        settings->loadExternal();
        settings->isError();
    }*/

    if ( settings->isLoaded() )
    {
        // set Library Path
        if ( settings->getLibraryPath()->isEmpty() )
        {

            libraryPath = new QString( QStandardPaths::displayName(QStandardPaths::MusicLocation) );
        }
        else {

            libraryPath = new QString( *settings->getLibraryPath() );
        }


        // set db
        if ( settings->getDbName()->isEmpty() )
            library_db = new DbManager(QStandardPaths::displayName(QStandardPaths::MusicLocation)+"/library.db");
        else {
            library_db = new DbManager( libraryPath + QChar('/') + *settings->getDbName() );
        }


        // set default Columns
        if ( settings->getDefaultColumns()->isEmpty() )
        {
            defaultColumns = new QStringList();
            defaultColumns->append("Track");
            defaultColumns->append("Time");
            defaultColumns->append("Artist");
            defaultColumns->append("Album");
            defaultColumns->append("Genre");

        }
        else {
            defaultColumns = new QStringList( *settings->getDefaultColumns() );
        }

        // set volume
        volume = settings->getVolume();
        if ( settings->getWatchedFolders()->isEmpty() )
        {
            watchedFolders = new QStandardItemModel;
        }
        else {
            watchedFolders = new QStandardItemModel;
            QStringList folders = *settings->getWatchedFolders();

            while ( folders.size() > 0 )
            {
                addToWatchedFolders( folders.takeFirst() );
            }
        }
    }

    return true;
}

void MainWindow::on_addFolderButton_clicked()
{
        QList<QUrl> urls;
        urls << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first())
             << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).first());

        QFileDialog dialog;
        dialog.setSidebarUrls(urls);
        dialog.setFileMode(QFileDialog::DirectoryOnly);
        if ( dialog.exec() )
            addToWatchedFolders( dialog.selectedUrls() );
}
