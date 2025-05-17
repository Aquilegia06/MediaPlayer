#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QStandardItemModel>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_plus_clicked();
    void playNext();
    void playPrevious();
    void on_pushButton_Volume_clicked();
    void on_horizontalSlider_Audio_File_Duration_2_valueChanged(int value);

private:

    Ui::MainWindow *ui;
    QStandardItemModel *m_playListModel;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QList<QUrl> m_playlistUrls;
    int m_currentTrackIndex = -1;
     bool m_isMuted = false;

};
#endif // MAINWINDOW_H
