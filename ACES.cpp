#include "ACES.h"
#include "ui_ACES.h"

ACES::ACES(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ACES)
{
    ui->setupUi(this);
    this->showFullScreen();   // TODO make game fullscreen and add quit button to menu

    spotDiffGame = new SpotTheDiff(this);

    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    QRect availableGeometry = primaryScreen->availableGeometry();

    int verticalPixelSize = availableGeometry.height();
    int horizontalPixelSize = availableGeometry.width();
    qDebug() << verticalPixelSize;
    qDebug() << horizontalPixelSize;

    if (verticalPixelSize >= 1300) {
        // change image scale
        DifferenceLocations::SCALE_FACTOR = 2.0;
        qDebug() << "Scale factor updated to " << DifferenceLocations::SCALE_FACTOR;

        // Resize scenes and views
        spotDiffGame->adjustSceneSizes(DifferenceLocations::SCALE_FACTOR);
    }

    ui->stackedWidget->addWidget(spotDiffGame);
    connect(spotDiffGame, SIGNAL(homeClicked()), this, SLOT(moveHome()));
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

