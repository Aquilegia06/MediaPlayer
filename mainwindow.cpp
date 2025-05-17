#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_playListModel(new QStandardItemModel(this))
    , m_player(new QMediaPlayer(this))
    , m_audioOutput(new QAudioOutput(this))
{
    ui->setupUi(this);


    ui->pushButton_Play_3->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButton_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButton_Seek_Back->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pushButton_Seek_forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    ui->horizontalSlider_Audio_File_Duration_2->setRange(0, 100);
    ui->horizontalSlider_Audio_File_Duration_2->setValue(70);
    m_audioOutput->setVolume(0.7);


    m_playListModel->setHorizontalHeaderLabels({tr("Аудио трек"), tr("Путь к файлу")});
    ui->playlistView->setModel(m_playListModel);
    ui->playlistView->hideColumn(1);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    m_audioOutput->setVolume(70);
    m_player->setAudioOutput(m_audioOutput);


    connect(ui->pushButton_Seek_Back, &QPushButton::clicked, this, &MainWindow::playPrevious);
    connect(ui->pushButton_Seek_forward, &QPushButton::clicked, this, &MainWindow::playNext);
    connect(ui->pushButton_Play_3, &QPushButton::clicked, m_player, &QMediaPlayer::play);
    connect(ui->pushButton_Pause, &QPushButton::clicked, m_player, &QMediaPlayer::pause);
    connect(ui->pushButton_Stop, &QPushButton::clicked, m_player, &QMediaPlayer::stop);
    connect(ui->pushButton_Volume, &QPushButton::clicked, this, &MainWindow::on_pushButton_Volume_clicked);

    connect(ui->playlistView, &QTableView::doubleClicked, this, [this](const QModelIndex &index){
        m_currentTrackIndex = index.row();
        m_player->setSource(m_playlistUrls[m_currentTrackIndex]);
        m_player->play();
    });

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
        if(status == QMediaPlayer::EndOfMedia) {
            playNext();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_plus_clicked()
{
    auto files = QFileDialog::getOpenFileNames(this, tr("Открыть файлы"), {}, tr("Аудио файлы (*.mp3 *.wav *.ogg)"));

    for (const auto &filePath : files) {
        QList<QStandardItem*> items {
            new QStandardItem(QFileInfo(filePath).fileName()),
            new QStandardItem(filePath)
        };
        m_playListModel->appendRow(items);
        m_playlistUrls.append(QUrl::fromLocalFile(filePath));
    }
}

void MainWindow::playNext()
{
    if(m_playlistUrls.isEmpty()) return;

    m_currentTrackIndex = (m_currentTrackIndex + 1) % m_playlistUrls.size();
    m_player->setSource(m_playlistUrls[m_currentTrackIndex]);
    m_player->play();
    ui->currentTrack->setText(m_playListModel->data(m_playListModel->index(m_currentTrackIndex, 0)).toString());
}

void MainWindow::playPrevious()
{
    if(m_playlistUrls.isEmpty()) return;

    m_currentTrackIndex = (m_currentTrackIndex - 1 + m_playlistUrls.size()) % m_playlistUrls.size();
    m_player->setSource(m_playlistUrls[m_currentTrackIndex]);
    m_player->play();
    ui->currentTrack->setText(m_playListModel->data(m_playListModel->index(m_currentTrackIndex, 0)).toString());
}



void MainWindow::on_pushButton_Volume_clicked()
{
    m_isMuted = !m_isMuted;
    m_audioOutput->setMuted(m_isMuted);


    ui->pushButton_Volume->setIcon(style()->standardIcon(
        m_isMuted ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));
}
void MainWindow::on_horizontalSlider_Audio_File_Duration_2_valueChanged(int value)
{

    m_audioOutput->setVolume(value / 100.0);


    if (value > 0 && m_isMuted) {
        m_isMuted = false;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}

