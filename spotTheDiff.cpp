#include "spotTheDiff.h"
#include "ui_spotTheDiff.h"

SpotTheDiff::SpotTheDiff(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpotTheDiff)
{
    ui->setupUi(this);

    gameTimer = new QTimer(this);
    measurement = new MeasurementModule(this, 0);   // 0 sets game type to Spot the Diff

    connect(gameTimer, SIGNAL(timeout()), this, SLOT(advanceTimerDisplay()));
    connect(this, SIGNAL(gameFinished()), this, SLOT(endGame()));
    connect(this, SIGNAL(gameStarted()), measurement, SLOT(startCount()));
    //connect(this, SIGNAL(gameStarted()), measurement, SLOT(logEvent()));
    connect(this, SIGNAL(differenceFound()), measurement, SLOT(logEvent()));

    connect(&imageScene, &ImageScene::clicked, this, &SpotTheDiff::sceneClicked);
    connect(&diffScene, &ImageScene::clicked, this, &SpotTheDiff::sceneClicked);

    // Install event filters to prevent scrolling on images
    ui->imageView->viewport()->installEventFilter(this);
    ui->diffView->viewport()->installEventFilter(this);
}

SpotTheDiff::~SpotTheDiff()
{
    delete ui;
}

// Updates image scene
void SpotTheDiff::updateImageScene(const QPixmap &pixmap)
{
    imageScene.addPixmap(pixmap.scaled(IMAGE_WIDTH*DifferenceLocations::SCALE_FACTOR,IMAGE_HEIGHT*DifferenceLocations::SCALE_FACTOR));
}

// Updates differing image scene
void SpotTheDiff::updateDiffScene(const QPixmap &pixmap)
{
    diffScene.addPixmap(pixmap.scaled(IMAGE_WIDTH*DifferenceLocations::SCALE_FACTOR,IMAGE_HEIGHT*DifferenceLocations::SCALE_FACTOR));
}

// Gets the next spot the difference image to be displayed
SpotTheDiff::img SpotTheDiff::getNextImage()
{
    //int value = QRandomGenerator::global()->bounded(NUM_IMAGES);
    static int value = 0;

    if (imagesRemaining == 0) {
        // Currently reset the images viewed status
        imagesRemaining = NUM_IMAGES;

        for (int i = 0; i < NUM_IMAGES; ++i) {
            imageArray.viewed[i] = false;
        }

        value = 0;
        // Eventually make the game end
    }

    // For some reason if Donut is displayed first, one of the diffs is almost impossible to see
//    while (imagesRemaining == NUM_IMAGES && value == Donut) {
//        value = QRandomGenerator::global()->bounded(NUM_IMAGES);
//    }

//    while (imageArray.viewed[value] == true && imagesRemaining != 0) {
//       value = QRandomGenerator::global()->bounded(NUM_IMAGES);
//    }


    imageArray.viewed[value] = true;
    imagesRemaining--;

    return static_cast<img>(value++);
}

void SpotTheDiff::updateImages(img select)
{
    updateImageScene(imageArray.normalImg[select]);
    updateDiffScene(imageArray.diffImg[select]);

    // Add images to view
    ui->imageView->setScene(&imageScene);
    ui->diffView->setScene(&diffScene);

    qDebug() << "Images updated to " << select;
}

void SpotTheDiff::initializeLists(img select)
{
    // Clear coordinates for fresh difference data
    coords.coordinateList.clear();
    coords.sizeList.clear();

    // Copy difference data to coordinate lists for selected image
    switch(select){

        case Balloons:

            coords.coordinateList = DifferenceLocations::balloonCoordList;
            coords.sizeList = DifferenceLocations::balloonSizeList;
            break;

        case Banana:

            coords.coordinateList = DifferenceLocations::bananaCoordList;
            coords.sizeList = DifferenceLocations::bananaSizeList;
            break;

        case Beach:

            coords.coordinateList = DifferenceLocations::beachCoordList;
            coords.sizeList = DifferenceLocations::beachSizeList;
            break;

        case Cupcakes:

            coords.coordinateList = DifferenceLocations::cupcakesCoordList;
            coords.sizeList = DifferenceLocations::cupcakesSizeList;
            break;

        case Donut:

            coords.coordinateList = DifferenceLocations::donutCoordList;
            coords.sizeList = DifferenceLocations::donutSizeList;
            break;

        case Honey:

            coords.coordinateList = DifferenceLocations::honeyCoordList;
            coords.sizeList = DifferenceLocations::honeySizeList;
            break;

        case Laptop:

            coords.coordinateList = DifferenceLocations::laptopCoordList;
            coords.sizeList = DifferenceLocations::laptopSizeList;
            break;

        case Sushi:

            coords.coordinateList = DifferenceLocations::sushiCoordList;
            coords.sizeList = DifferenceLocations::sushiSizeList;
            break;

        case Twine:

            coords.coordinateList = DifferenceLocations::twineCoordList;
            coords.sizeList = DifferenceLocations::twineSizeList;
            break;

        case Vegetable:

            coords.coordinateList = DifferenceLocations::vegetableCoordList;
            coords.sizeList = DifferenceLocations::vegetableSizeList;
            break;
    }

}

void SpotTheDiff::loadDiffItems()
{
    for (int i = 0; i < coords.coordinateList.size(); ++i) {
        QPoint coordData = coords.coordinateList[i];
        QSize sizeData = coords.sizeList[i];

        // Make difference item for each scene
        DifferenceItem *diff1 = new DifferenceItem(coordData.x(),coordData.y(),sizeData.width(),sizeData.height());
        DifferenceItem *diff2 = new DifferenceItem(coordData.x(),coordData.y(),sizeData.width(),sizeData.height());

        // Add to list of difference items
        differenceItems.append(diff1);
        differenceItems.append(diff2);

        // Add items to scenes
        imageScene.addItem(diff1);
        diffScene.addItem(diff2);

        // Debug info
        qDebug() << "X: " << coords.coordinateList[i].x()
                 << ", Y: " << coords.coordinateList[i].y();
    }
}

void SpotTheDiff::removeItems()
{
    // Disconnect Signal-Slot
    for (DifferenceItem* diffItem : differenceItems) {
        disconnect(diffItem, &DifferenceItem::differenceClicked, this, &SpotTheDiff::highlightCorrespondingItem);
    }

    // Clear lists and scenes
    differenceItems.clear();
    imageScene.clear();
    diffScene.clear();

    qDebug() << "GraphicsScenes cleared";
}

void SpotTheDiff::scaleDiffPoints()
{
    for (int i = 0; i < coords.coordinateList.size(); ++i) {
        coords.coordinateList[i] *= DifferenceLocations::SCALE_FACTOR;
        coords.sizeList[i] *= DifferenceLocations::SCALE_FACTOR;
    }
}

void SpotTheDiff::centerDiffOrigins()
{
    // Center the coordinates in the circle
    for (int i = 0; i < coords.coordinateList.size(); ++i) {
        coords.coordinateList[i] = QPoint(coords.coordinateList[i].x()-coords.sizeList[i].width()/2,
                         coords.coordinateList[i].y()-coords.sizeList[i].height()/2);
    }
}

void SpotTheDiff::connectScenes()
{
    // Iterate through the differenceItems and connect signals
    for (DifferenceItem* diffItem : differenceItems) {
            connect(diffItem, &DifferenceItem::differenceClicked, this, &SpotTheDiff::highlightCorrespondingItem);
    }
    // TODO connect a scene click to method for incorrect click output

    connect(this, SIGNAL(differenceFound()), this, SLOT(advanceDifferencesDisplay()));
}

void SpotTheDiff::highlightCorrespondingItem() {
    // Identify the clicked item
    DifferenceItem* clickedItem = qobject_cast<DifferenceItem*>(sender());

    if (!clickedItem) {
            return;
    }

    if (clickedItem->highlighted == false) {
            emit differenceFound();
            measurement->logEvent(MeasurementModule::correct);
    }

    // Highlight the corresponding item in the other scene
    if (imageScene.items().contains(clickedItem)) {
            // Item is in imageScene, find and highlight in diffScene
            for (QGraphicsItem* item : diffScene.items()) {
                DifferenceItem* diffItem = qgraphicsitem_cast<DifferenceItem*>(item);
                if (diffItem->correspondsTo(clickedItem)) {
                    // Highlight the item in diffScene
                    diffItem->highlight();
                    clickedItem->highlight();
                    ui->diffView->viewport()->update();
                    break;
                }
            }
    } else if (diffScene.items().contains(clickedItem)) {
            // Item is in diffScene, find and highlight in imageScene
            for (QGraphicsItem* item : imageScene.items()) {
                DifferenceItem* imageItem = qgraphicsitem_cast<DifferenceItem*>(item);
                if (imageItem->correspondsTo(clickedItem)) {
                    // Highlight the item in imageScene
                    imageItem->highlight();
                    clickedItem->highlight();
                    ui->imageView->viewport()->update();
                    break;
                }
            }
    }
}

void SpotTheDiff::updateItemLabels()
{
    QString remaining = QString("Remaining: %1").arg(itemsRemaining);
    ui->RemainingLabel->setText(remaining);

    QString found = QString("Spotted: %1").arg(itemsFound);
    ui->SpottedLabel->setText(found);
}

void SpotTheDiff::initGame()
{
    form = new ReadyForm(this);

    ui->stackedWidget->addWidget(form);
    connect(form, SIGNAL(startGame()), this, SLOT(StartGame()));

    // Set the ready form to be the visible widget
    //form
    ui->stackedWidget->setCurrentIndex(1);
}

void SpotTheDiff::endGame()
{
    gameTimer->stop();
    qDebug() << "Game Over";


    form = new ReadyForm(this);
    form->changeScreen(2);

    int timeTaken = GAME_LENGTH_SECONDS - countdownSeconds;

    int minutes = timeTaken / 60;
    int seconds = timeTaken % 60;

    QString found = QString("You found %1 differences in %2:%3")
                        .arg(itemsFound)
                        .arg(minutes, 1, 10, QLatin1Char('0'))
                        .arg(seconds, 2, 10, QLatin1Char('0'));

    qDebug() << found;

    form->changeResultLabel(found);

    connect(form, SIGNAL(homeButtonClicked()), this, SLOT(exitGame()));
    connect(form, SIGNAL(startGame()), this, SLOT(StartGame()));

    // Change display if user didn't find all differences
    if (itemsRemaining > 0)
    {
            form->changeEndLabel("Almost!");
            form->showRestartOnGameEnd();
    }

    ui->stackedWidget->addWidget(form);
    ui->stackedWidget->setCurrentWidget(form);

}

void SpotTheDiff::exitGame()
{
    delete form;
    emit homeClicked();
}

void SpotTheDiff::sceneClicked()
{
    measurement->logEvent(MeasurementModule::incorrect);
}

void SpotTheDiff::StartGame()
{
    // Get new image if restart has not been selected
    if (form->getRestartValue() == false) select = getNextImage();

    disconnect(this, SIGNAL(differenceFound()), this, SLOT(advanceDifferencesDisplay()));

    // Set game widget to current viewed widget
    ui->stackedWidget->setCurrentWidget(ui->Game);
    qDebug() << "set widget to game";

    // Remove old instance of the ready form
    ui->stackedWidget->removeWidget(form);
    delete form;

    // Remove old difference items
    removeItems();

    // Update images based on selection
    updateImages(select);
    initializeLists(select);

    scaleDiffPoints();
    centerDiffOrigins();

    loadDiffItems();
    connectScenes();

    itemsRemaining = coords.coordinateList.size();
    itemsFound = 0;
    updateItemLabels();

    countdownSeconds = GAME_LENGTH_SECONDS;
    ui->TimerLabel->setText(GAME_MAX_TIME);
    gameTimer->start(TIMER_INTERVAL);

    emit gameStarted();
    measurement->logEvent(MeasurementModule::STDinfo);

}

void SpotTheDiff::advanceDifferencesDisplay()
{
    itemsRemaining--;
    itemsFound++;

    updateItemLabels();

    if (itemsRemaining == 0) {
        measurement->logEvent(MeasurementModule::eventType::STDinfo);
        emit gameFinished();
            //return;
    }

}

void SpotTheDiff::advanceTimerDisplay()
{
    if (countdownSeconds > 0) {
            countdownSeconds--;

            int minutes = countdownSeconds / 60;
            int remainingSeconds = countdownSeconds % 60;

            QString timeString = QString("%1:%2")
                                     .arg(minutes, 2, 10, QLatin1Char('0'))
                                     .arg(remainingSeconds, 2, 10, QLatin1Char('0'));

            ui->TimerLabel->setText(timeString);
    }
    else {
            gameTimer->stop();
            measurement->logEvent(MeasurementModule::eventType::STDinfo);
            emit gameFinished();
    }
}


void SpotTheDiff::adjustSceneSizes(qreal scaleFactor)
{
    imageScene.setSceneRect(imageScene.sceneRect().x(), imageScene.sceneRect().y(),
                            IMAGE_WIDTH * scaleFactor, IMAGE_HEIGHT * scaleFactor);
    diffScene.setSceneRect(diffScene.sceneRect().x(), diffScene.sceneRect().y(),
                            IMAGE_WIDTH * scaleFactor, IMAGE_HEIGHT * scaleFactor);

    ui->imageView->setFixedSize(imageScene.width(), imageScene.height());
    ui->diffView->setFixedSize(diffScene.width(), diffScene.height());

    imageScene.update();
    ui->imageView->viewport()->update();
    ui->diffView->viewport()->update();
}


void SpotTheDiff::on_homeButton_clicked()
{
    endGame();
    exitGame();
}


void SpotTheDiff::on_restartButton_clicked()
{
    initGame();
    form->changeScreen(1);
}
