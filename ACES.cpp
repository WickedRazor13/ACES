#include "ACES.h"
#include "ui_ACES.h"

BluetoothManager *ACESBLUE::blue = nullptr;

ACES::ACES(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ACES)
{
    ui->setupUi(this);
    this->showFullScreen();

    spotDiffGame = new SpotTheDiff(this);
    MemoryGame = new memory(this);
    //Tetrizz = new Widget(this);
    ACESBLUE::blue = new BluetoothManager(this);

    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    QRect availableGeometry = primaryScreen->availableGeometry();

    int verticalPixelSize = availableGeometry.height();
    int horizontalPixelSize = availableGeometry.width();
    qDebug() << verticalPixelSize;
    qDebug() << horizontalPixelSize;

    qreal pixelRatio = QApplication::primaryScreen()->devicePixelRatio();
    qDebug() << pixelRatio;

    if (verticalPixelSize >= 1300) {
        // change image scale
        DifferenceLocations::SCALE_FACTOR = 2.0;
        qDebug() << "Scale factor updated to " << DifferenceLocations::SCALE_FACTOR;

        // Resize scenes and views
        spotDiffGame->adjustSceneSizes(DifferenceLocations::SCALE_FACTOR);
    }

    ui->stackedWidget->addWidget(spotDiffGame);
    ui->stackedWidget->addWidget(MemoryGame);
    //ui->stackedWidget->addWidget(Tetrizz);

    connect(spotDiffGame, SIGNAL(homeClicked()), this, SLOT(moveHome()));
    connect(MemoryGame, SIGNAL(homeClicked()), this, SLOT(moveHome()));
    //connect(Tetrizz, SIGNAL(homeClicked()), this, SLOT(moveHome()));
}

ACES::~ACES()
{
    delete spotDiffGame;
    delete ui;
}

void ACES::on_spotDiffButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(spotDiffGame);
    spotDiffGame->initGame();
    //ui->stackedWidget->setCurrentIndex(1);
    // start game method call here

}

void ACES::moveHome()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void ACES::on_quitButton_clicked()
{
    QApplication::quit();
}


void ACES::on_memoryButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(MemoryGame);
    MemoryGame->showForm();
}


void ACES::on_tetrisButton_clicked()
{
    Tetrizz = new Widget(this);
    ui->stackedWidget->addWidget(Tetrizz);
    connect(Tetrizz, SIGNAL(homeClicked()), this, SLOT(moveHome()));
    ui->stackedWidget->setCurrentWidget(Tetrizz);
    Tetrizz->StartGame();
}

