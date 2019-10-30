#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QTableWidgetItem>
#include <QDir>
#include <QStandardItemModel>
#include "dbmanager.h"
#include "settings.h"

#define MIN 0
#define MED 1
#define MAX 2

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_dashboardButton_clicked();
    void on_searchButton_clicked();
    void on_volumeSlider_valueChanged(int value);
    void on_libraryButton_clicked();
    void on_closeButton_clicked();
    void positionChanged( qint64 position );
    void durationChanged( qint64 position );
    void on_playButton_clicked();
    void playButtonState( QMediaPlayer::State state );
    void updateTrackInfo(bool available);
    void on_libraryTable_itemDoubleClicked(QTableWidgetItem *item);
    void on_settingsButton_clicked();
    void on_addFolderButton_clicked();

private:
    void initDB();
    void importFolder( QDir *parentFolder );
    void importFolder( QUrl *parentFolder );
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    void addToWatchedFolders( QString );
    void addToWatchedFolders( QList<QUrl> folderList );
    bool isConfigLoaded();
    void updateLibrary();

    bool readConfig();

    DbManager *library_db = nullptr;
    Settings *settings = nullptr;

    QDir *libraryDir = nullptr;
    QString *libraryPath = nullptr;
    QStringList *defaultColumns;
    int volume = 0;

    QIcon volumeIcons[3] = {
        QIcon(":/icons/volume/icons/volume-white-min.png"),
        QIcon(":/icons/volume/icons/volume-white-med.png"),
        QIcon(":/icons/volume/icons/volume-white-max.png")
    };

    QIcon playButtonIcons[3] = {
        QIcon(":/icons/playercontrols/icons/play.png"),
        QIcon(":/icons/playercontrols/icons/play.png"),
        QIcon(":/icons/playercontrols/icons/pause.png")
    };

    QFile *configFile;
    QStandardItemModel *watchedFolders;

signals:
    void watchedFoldersChanged( QString &newFolder );
};
#endif // MAINWINDOW_H
