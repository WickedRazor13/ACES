#include "memory.h"
#include "ui_memory.h"

memory::memory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::memory)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    gameTimer = new QTimer(this);

    // Set starting game state
    currentState = state::Display;

    connect(gameTimer, SIGNAL(timeout()), this, SLOT(advanceTimer()));
    connect(this, SIGNAL(timerFinished()), this, SLOT(advanceNum()));
    connect(this, SIGNAL(stateFinished()), this, SLOT(advanceGame()));


    // Setup Timer display
    countdownSeconds = DISPLAY_TIME;

    initGame();
    gameTimer->start(TIMER_INTERVAL);
}

memory::~memory()
{
    delete ui;
}

void memory::initGame()
{
    ui->restartButton->hide();

    // Shuffle number vectors
    QRandomGenerator rng;
    std::shuffle(seenNum.begin(), seenNum.end(), rng);
    std::shuffle(newNum.begin(), newNum.end(), rng);

    displayNum = seenNum[0];
    ui->NumLabel->setText(QString::number(displayNum));
}

void memory::compareNums(const QString &str1, const QString &str2)
{
    correct = 0;

    int minLength = qMin(str1.length(), str2.length());

    for (int i = 0; i < minLength; ++i) {
        if (str1.at(i) == str2.at(i)) ++correct;
    }
}

void memory::endGame()
{
    gameTimer->stop();
}

void memory::exitGame()
{
    emit homeClicked();
}

void memory::advanceTimer()
{
    if (countdownSeconds > 0) {
        countdownSeconds--;
        qDebug() << countdownSeconds;
    }
    else {
        gameTimer->stop();
        //currentState = static_cast<state>(static_cast<int>(currentState)+1);
        emit timerFinished();
    }
}

void memory::advanceNum()
{
    if (index < NUM_NUM) {
        displayNum = seenNum[++index];
        ui->NumLabel->setText(QString::number(displayNum));
        countdownSeconds = DISPLAY_TIME;
        gameTimer->start(TIMER_INTERVAL);
    }
    else {
        currentState = static_cast<state>(static_cast<int>(currentState)+1);
        emit stateFinished();
    }
}

void memory::advanceGame()
{
    switch(currentState) {
        case Display:
            initGame();
            ui->label->setText("Study the numbers below.");
            countdownSeconds = DISPLAY_TIME;
            gameTimer->start(TIMER_INTERVAL);
            ui->NumLabel->show();
            break;
        case Hide:
            countdownSeconds = DOWN_TIME;
            gameTimer->start(TIMER_INTERVAL);
            ui->NumLabel->hide();
            ui->label->hide();
            break;
        case Test:
            countdownSeconds = TEST_TIME;
            ui->label->setText("Is the number new or have you seen it before?");
            ui->label->show();
            gameTimer->start(TIMER_INTERVAL);
            break;
        case StateCount:
            // Display endgame stuff
            // Reset cycle
            QString info = "You matched " + QString::number(correct)
                           + " out of " + QString::number(RNG_LENGTH);

            ui->label->setText(info);
            currentState = Display;
            ui->restartButton->show();

    }
}

void memory::on_homeButton_clicked()
{
    endGame();
    exitGame();
}


void memory::on_lineEdit_returnPressed()
{
    gameTimer->stop();
    currentState = static_cast<state>(static_cast<int>(currentState)+1);

    qDebug() << userNum << '\n';

    QString num = QString::number(displayNum);

    compareNums(num, userNum);

    emit timerFinished();
}


void memory::on_restartButton_clicked()
{
    emit timerFinished();
}

