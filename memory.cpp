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

    countdownSeconds = DISPLAY_TIME;

    //showForm();
}

memory::~memory()
{
    delete ui;
}

void memory::showForm()
{
    form = new ReadyForm(this);

    form->changeScreen(3);
    ui->stackedWidget->addWidget(form);
    connect(form, SIGNAL(startMemory()), this, SLOT(StartGame()));

    // Set the ready form to be the visible widget
    //form

    ui->stackedWidget->setCurrentIndex(1);
}



void memory::initGame()
{
    ui->restartButton->hide();
    ui->diffButtonFrame->hide();

    // Shuffle number vectors
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    QRandomGenerator rng(seed);
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
        displayNum = seenNum[index++];
        qDebug() << index;
        ui->NumLabel->setText(QString::number(displayNum));
        countdownSeconds = DISPLAY_TIME;
        gameTimer->start(TIMER_INTERVAL);
    }
    else {
        currentState = static_cast<state>(static_cast<int>(currentState)+1);
        emit stateFinished();
    }
}

// TODO: Advance TEST method - should be tied to button clicks

void memory::advanceGame()
{
    switch(currentState) {
        case Display:
            showForm();
            ui->label->setText("Study the numbers below.");
            countdownSeconds = DISPLAY_TIME;
            gameTimer->start(TIMER_INTERVAL);
            ui->NumLabel->show();
            break;
        case Hide:
            countdownSeconds = DOWN_TIME;
            gameTimer->start(TIMER_INTERVAL);
            ui->NumLabel->hide();
            ui->label->setText("Have a short break :)");
            break;
        case Test:
            countdownSeconds = TEST_TIME;
            ui->label->setText("Is the number new or have you seen it before?");
            ui->NumLabel->show();
            ui->diffButtonFrame->show();

            // Start and advance game
            gameTimer->start(TIMER_INTERVAL);
            break;
        case StateCount:
            // Display endgame stuff
            // Reset cycle
            QString info = "You matched " + QString::number(correct)
                           + " out of " + QString::number(5);

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

void memory::StartGame()
{
    initGame();
    ui->stackedWidget->setCurrentIndex(0);
    gameTimer->start(TIMER_INTERVAL);

    // Remove old instance of the ready form
    ui->stackedWidget->removeWidget(form);
    delete form;
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

