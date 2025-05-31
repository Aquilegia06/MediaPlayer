#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPushButton>
#include <QResizeEvent>
#include <QBoxLayout>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_playListModel(new QStandardItemModel(this))
    , m_player(new QMediaPlayer(this))
    , m_audioOutput(new QAudioOutput(this))
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
}

void MainWindow::setupUI()
{

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);


    ui->pushButton_plus->setText(tr("Добавить"));
    ui->pushButton_plus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


    ui->currentTrack->setText(tr("НАЗВАНИЕ ПЕСНИ"));
    ui->currentTrack->setAlignment(Qt::AlignCenter);
    QFont trackFont = ui->currentTrack->font();
    trackFont.setPointSize(14);
    trackFont.setBold(true);
    ui->currentTrack->setFont(trackFont);


    QHBoxLayout* controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(5);


    QSize iconSize(40, 40);
    auto setupButton = [iconSize](QPushButton* btn) {
        btn->setText("");
        btn->setMinimumSize(iconSize);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    };

    ui->pushButton_Seek_Back->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    setupButton(ui->pushButton_Seek_Back);

    ui->pushButton_Play_3->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    setupButton(ui->pushButton_Play_3);

    ui->pushButton_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    setupButton(ui->pushButton_Pause);

    ui->pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    setupButton(ui->pushButton_Stop);

    ui->pushButton_Seek_forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    setupButton(ui->pushButton_Seek_forward);


    controlsLayout->addWidget(ui->pushButton_Seek_Back);
    controlsLayout->addWidget(ui->pushButton_Play_3);
    controlsLayout->addWidget(ui->pushButton_Pause);
    controlsLayout->addWidget(ui->pushButton_Stop);
    controlsLayout->addWidget(ui->pushButton_Seek_forward);


    ui->horizontalSlider_Volume->setRange(0, 100);
    ui->horizontalSlider_Volume->setValue(70);
    ui->horizontalSlider_Volume->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->pushButton_Volume->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->pushButton_Volume->setMinimumSize(iconSize);

    QHBoxLayout* volumeLayout = new QHBoxLayout();
    volumeLayout->addWidget(ui->pushButton_Volume);
    volumeLayout->addWidget(ui->horizontalSlider_Volume);


    m_playListModel->setHorizontalHeaderLabels({tr(""), tr("Путь к файлу")});
    ui->playlistView->setModel(m_playListModel);
    ui->playlistView->hideColumn(1);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    mainLayout->addWidget(ui->pushButton_plus);
    mainLayout->addWidget(ui->currentTrack);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(ui->playlistView, 1);
    mainLayout->addLayout(volumeLayout);


    centralWidget->setStyleSheet(R"(
        QWidget {
            background-color: #2e2f30;
        }
        QPushButton {
            color: white;
            background: #3a3b3c;
            border-radius: 5px;
            min-height: 40px;
            padding: 5px;
        }
        QPushButton:hover {
            background: #4a4b4c;
        }
        QPushButton#pushButton_plus {
            text-align: left;
            padding-left: 15px;
            font-size: 12px;
        }
        QTableView {
            background: #2e2f30;
            color: white;
            alternate-background-color: #3a3b3c;
            border: 1px solid #4a4b4c;
        }
        QLabel {
            color: white;
            font-size: 14px;
            padding: 10px;
        }
        QSlider::groove:horizontal {
            height: 5px;
            background: #4a4b4c;
        }
        QSlider::handle:horizontal {
            background: white;
            width: 15px;
            height: 15px;
            margin: -5px 0;
            border-radius: 7px;
        }
    )");


    ui->pushButton_plus->setObjectName("pushButton_plus");
}

void MainWindow::setupConnections()
{
    m_audioOutput->setVolume(0.7);
    m_player->setAudioOutput(m_audioOutput);

    connect(ui->pushButton_Seek_Back, &QPushButton::clicked, this, &MainWindow::playPrevious);
    connect(ui->pushButton_Seek_forward, &QPushButton::clicked, this, &MainWindow::playNext);
    connect(ui->pushButton_Play_3, &QPushButton::clicked, m_player, &QMediaPlayer::play);
    connect(ui->pushButton_Pause, &QPushButton::clicked, m_player, &QMediaPlayer::pause);
    connect(ui->pushButton_Stop, &QPushButton::clicked, m_player, &QMediaPlayer::stop);
    connect(ui->pushButton_Volume, &QPushButton::clicked, this, &MainWindow::on_pushButton_Volume_clicked);
    connect(ui->horizontalSlider_Volume, &QSlider::valueChanged, this, &MainWindow::on_horizontalSlider_Volume_valueChanged);

    connect(ui->playlistView, &QTableView::doubleClicked, this, [this](const QModelIndex &index){
        m_currentTrackIndex = index.row();
        m_player->setSource(m_playlistUrls[m_currentTrackIndex]);
        m_player->play();
        ui->currentTrack->setText(m_playListModel->data(m_playListModel->index(m_currentTrackIndex, 0)).toString());
    });

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
        if(status == QMediaPlayer::EndOfMedia) {
            playNext();
        }
    });
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    QFont font = ui->currentTrack->font();
    font.setPixelSize(qMax(12, height() / 30));
    ui->currentTrack->setFont(font);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_plus_clicked()
{
    auto files = QFileDialog::getOpenFileNames(this, tr("Открыть файлы"),
                                               QDir::homePath(), tr("Аудио файлы (*.mp3 *.wav *.ogg)"));

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

void MainWindow::on_horizontalSlider_Volume_valueChanged(int value)
{
    m_audioOutput->setVolume(value / 100.0);
    if (value > 0 && m_isMuted) {
        m_isMuted = false;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}
