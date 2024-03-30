#ifndef SPOTTHEDIFF_H
#define SPOTTHEDIFF_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>
#include <QPoint>
#include <QList>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QSize>
#include <QScreen>
#include <QVector>
#include <QWheelEvent>
#include "differenceLocations.h"
#include "differenceItem.h"
#include "readyForm.h"
#include "measurementModule.h"
#include "imagescene.h"

// Image dimensions - image is 350 x 467 px at 100%
const int IMAGE_WIDTH = 350;
const int IMAGE_HEIGHT = 467;

const int NUM_IMAGES = 10;

const int GAME_LENGTH_SECONDS = 180;
const QString GAME_MAX_TIME = "03:00";

namespace Ui {
class SpotTheDiff;
}

class SpotTheDiff : public QWidget
{
    Q_OBJECT

public:

    // Enumerated type for all the images
    enum img {
        Balloons = 0,
        Banana,
        Beach,
        Cupcakes,
        Donut,
        Honey,
        Laptop,
        Sushi,
        Twine,
        Vegetable
    };
    Q_ENUM(img)

    struct coordinates {     // Coordinate structure for holding image difference points
        QList<QPoint> coordinateList;   // List of difference coordinates per image
        QList<QSize> sizeList;          // Width and height of difference (for drawing clickable objects)
    };

    struct images {     // Image structure for images and their different counterparts TODO MAYBE MAKE CONSTANT?
        QPixmap normalImg[NUM_IMAGES] = {QPixmap(":/images/balloons.jpg"),
                                         QPixmap(":/images/bananaBread.jpg"),
                                         QPixmap(":/images/beach.jpg"),
                                         QPixmap(":/images/cupcakes.jpg"),
                                         QPixmap(":/images/donut.jpg"),
                                         QPixmap(":/images/honey.jpg"),
                                         QPixmap(":/images/laptop.jpg"),
                                         QPixmap(":/images/sushi.jpg"),
                                         QPixmap(":/images/twine.jpg"),
                                         QPixmap(":/images/vegetable.jpg")};      // Normal images

        QPixmap diffImg[NUM_IMAGES] =   {QPixmap(":/images/balloonsDiff.jpg"),
                                         QPixmap(":/images/bananaBreadDiff.jpg"),
                                         QPixmap(":/images/beachDiff.jpg"),
                                         QPixmap(":/images/cupcakesDiff.jpg"),
                                         QPixmap(":/images/donutDiff.jpg"),
                                         QPixmap(":/images/honeyDiff.jpg"),
                                         QPixmap(":/images/laptopDiff.jpg"),
                                         QPixmap(":/images/sushiDiff.jpg"),
                                         QPixmap(":/images/twineDiff.jpg"),
                                         QPixmap(":/images/vegetableDiff.jpg")};         // Differing image

        bool viewed[NUM_IMAGES] = {false};
    };


    explicit SpotTheDiff(QWidget *parent = nullptr);
    ~SpotTheDiff();

    void updateImageScene(const QPixmap& pixmap);
    void updateDiffScene(const QPixmap &pixmap);

    img getNextImage();     // Selects the next image and returns enum for use in other methods
    void updateImages(img select);      // calls updateScenes with the pixmaps based on getnextimage

    // Find the placements of, load, and remove difference items
    void initializeLists(img select);
    void loadDiffItems();
    void removeItems();

    // Scale and center the differences to match image scaling
    void scaleDiffPoints();
    void centerDiffOrigins();

    void adjustSceneSizes(qreal scaleFactor);    // Adjusts the size of the scene based on screen

    // Connects both image scenes to sync corresponding differences
    void connectScenes();
    void highlightCorrespondingItem();



    // Game logic
    void updateItemLabels();

    void initGame();    // Method potentially for when selecting std from menu and need to restart.



private:
    Ui::SpotTheDiff *ui;

    ReadyForm *form;

    MeasurementModule *measurement;

    images imageArray;      // Array of selectable images
    img select;             // selection variable for initlists func
    coordinates coords;     // list of difference placements

    int imagesRemaining = NUM_IMAGES;
    int itemsRemaining;
    int itemsFound;

    QTimer *gameTimer;
    int countdownSeconds;

    // Graphics Scenes
    //QGraphicsScene imageScene;
    //QGraphicsScene diffScene;
    ImageScene imageScene;
    ImageScene diffScene;

    // Objects to track differences
    QVector<DifferenceItem*> differenceItems;   // Dynamic vector for allocating differenceItems unique to each image



protected:
    // Scroll wheel event filter to prevent image movement
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::Wheel) {
            // Ignore the wheel event to prevent scrolling
            qDebug() << "Wheel event filtered";
            event->ignore();
            return true;
        }

        // Let the base class handle other events
        return QWidget::eventFilter(obj, event);
    }


signals:
    void homeClicked();
    void countdownFinished();
    void gameStarted();
    void gameFinished();
    void differenceFound();

private slots:
    void StartGame();
    void endGame();
    void exitGame();
    void sceneClicked();

    // GUI advancement methods
    void advanceDifferencesDisplay();
    void advanceTimerDisplay();

    void on_homeButton_clicked();

    void on_restartButton_clicked();

};

#endif // SPOTTHEDIFF_H
