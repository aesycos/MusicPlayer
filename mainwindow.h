#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QTableWidgetItem>
#include <QDir>
#include "dbmanager.h"

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

private:
    void initDB();
    void importFolder( QDir *parentFolder );
    void importFolder( QUrl *parentFolder );
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    DbManager *library_db;
    QDir *libraryDir;
    QString libraryPath;
    QStringList defaultColumns;
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
};
#endif // MAINWINDOW_H
