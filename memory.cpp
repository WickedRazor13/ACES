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
    connect(this, SIGNAL(selectionMade()), this, SLOT(processSelection()));

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
    ui->diffButtonFrame->hide();

    // Shuffle number vectors
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    QRandomGenerator rng(seed);
    std::shuffle(seenNum.begin(), seenNum.end(), rng);
    std::shuffle(newNum.begin(), newNum.end(), rng);

    displayNum = seenNum[0];
    ui->NumLabel->setText(QString::number(displayNum));
}

void memory::combineVectors(QVector<int> &oldVec, QVector<int> &newVec)
{
    for (int i = 0; i < NUM_NUM; i++) {
        bool pickFromVec1 = QRandomGenerator::global()->bounded(2); // Returns 0 or 1

        if (pickFromVec1 && !oldVec.isEmpty()) {
            testNum.append(oldVec.back());
            qDebug() << oldVec.back();
            oldVec.pop_back();    // Might need to change to pass by value so it doesn't mess with original vector
            Ans[i] = 0;     // Set to 0 for old number
        }
        else if (!newVec.isEmpty()) {
            testNum.append(newVec.back());
            qDebug() << newVec.back();
            newVec.pop_back();
            Ans[i] = 1;     // Set to 1 for new number
        }
    }
}

void memory::endGame()
{
    gameTimer->stop();
    currentState = Display;
    index = 0;
    advanceGame();
}

void memory::exitGame()
{
    emit homeClicked();
}

void memory::advanceTimer()
{
    if (countdownSeconds > 0) {
        countdownSeconds--;
        //qDebug() << countdownSeconds;
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

void memory::advanceGame()
{
    switch(currentState) {
        case Display:
            showForm();
            ui->label->setText("Study the numbers below.");
            countdownSeconds = DISPLAY_TIME;
            gameTimer->start(TIMER_INTERVAL);
            index = 0;
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
            ui->diffButtonFrame->show();
            index = 0;
            combineVectors(seenNum, newNum);
            displayNum = testNum[index];
            ui->NumLabel->setText(QString::number(displayNum));
            ui->NumLabel->show();
            //gameTimer->start(TIMER_INTERVAL);
            // Display timer at bottom // will need to reimplement timer display func
            break;
        case End:
            // Display endgame stuff
            // Reset cycle
            QString info = "You got " + QString::number(correct)
                           + " out of " + QString::number(NUM_NUM);

            ui->label->setText(info);
            currentState = Display;
            ui->diffButtonFrame->hide();
            ui->NumLabel->hide();
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
    index = 0;
    correct = 0;
    gameTimer->start(TIMER_INTERVAL);

    // Remove old instance of the ready form
    ui->stackedWidget->removeWidget(form);
    delete form;
}


void memory::on_oldButton_clicked()
{
    selection = 0;  // Old selection made
    emit selectionMade();
}


void memory::on_newButton_clicked()
{
    selection = 1;  // New selection made
    emit selectionMade();
}

void memory::processSelection()
{
    if (index < NUM_NUM) {
        qDebug() << testNum[index];
        qDebug() << "New?: " << Ans[index];
        qDebug() << "Selection: " << selection;

        if (selection == Ans[index]) {   // Compare user guess to actual value
        userAns[index] = 1;     // User made correct selection
        correct++;
        qDebug() << "correct\n";
        }
        else {
        userAns[index] = 0;
        qDebug() << "incorrect\n";
        }

        // Update number display
        displayNum = testNum[index++];
        ui->NumLabel->setText(QString::number(displayNum));
    }
    else {
        currentState = End;
        emit stateFinished();
    }
}




