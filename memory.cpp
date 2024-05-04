#include "memory.h"
#include "ui_memory.h"

// Forward declaration of ACESBLUE namespace
namespace ACESBLUE {
extern BluetoothManager *blue;
}

memory::memory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::memory)
{
    ui->setupUi(this);
    measurement = new MeasurementModule(this, 1);   // 1 Sets game type to memory

    ui->stackedWidget->setCurrentIndex(0);

    gameTimer = new QTimer(this);

    // Set starting game state
    currentState = state::Display;

    connect(gameTimer, SIGNAL(timeout()), this, SLOT(advanceTimer()));
    connect(this, SIGNAL(timerFinished()), this, SLOT(advanceNum()));
    connect(this, SIGNAL(stateFinished()), this, SLOT(advanceGame()));
    connect(this, SIGNAL(selectionMade()), this, SLOT(processSelection()));

    countdownSeconds = DISPLAY_TIME;

    ui->homeButton->hide();
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
    ui->TimerLabel->hide();

    // Shuffle number vectors
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    QRandomGenerator rng(seed);
    std::shuffle(seenNum.begin(), seenNum.end(), rng);
    std::shuffle(newNum.begin(), newNum.end(), rng);

    displayNum = seenNum[0];
    ui->NumLabel->setText(QString::number(displayNum));

    QFile logFile;
    logFile.setFileName("Known_numbers.csv");
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream stream(&logFile);

    for(int i = 0; i < NUM_NUM; i++) {
        stream << seenNum[i] << "\n";
    }

    logFile.close();
}

void memory::combineVectors(QVector<int> &oldVec, QVector<int> &newVec)
{
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    QRandomGenerator rng(seed);

    // Shuffle both number vectors
    std::shuffle(oldVec.begin(), oldVec.end(), rng);
    std::shuffle(newVec.begin(), newVec.end(), rng);

    // Fill test number vector
    for (int i = 0; i < NUM_NUM; i++) {
        if (i < NUM_NUM/2) {
            testNum.append(oldVec[i]); // Add known numbers
        } else {
            testNum.append(newVec[i]); // Add unknown numbers
        }
    }

    std::shuffle(testNum.begin(), testNum.end(), rng);  // Shuffle test number vector

    for (int i =0; i < NUM_NUM; i++) {
        if (oldVec.contains(testNum[i])){
            Ans.append(0);  // Set 0 for old number
        }
        else {
            Ans.append(1);  // Set 1 for new number
        }
    }

    QFile logFile;
    logFile.setFileName("Test_numbers.csv");
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream stream(&logFile);

    for(int i = 0; i < NUM_NUM; i++) {
        stream << testNum[i] << ", ";
        if (Ans[i] == 0) {
            stream << "known\n";
        } else if (Ans[i] == 1) {
            stream << "unknown\n";
        }
    }

    logFile.close();
}

void memory::endGame()
{
    gameTimer->stop();
    index = 0;
}

void memory::exitGame()
{
    emit homeClicked();
}

void memory::advanceTimer()
{
    if (countdownSeconds > 1) {
        countdownSeconds--;

        int minutes = countdownSeconds / 60;
        int remainingSeconds = countdownSeconds % 60;

        QString timeString = QString("%1:%2")
                                 .arg(minutes, 2, 10, QLatin1Char('0'))
                                 .arg(remainingSeconds, 2, 10, QLatin1Char('0'));

        ui->TimerLabel->setText(timeString);
        //qDebug() << countdownSeconds;
    }
    else if (currentState == Test) {
        gameTimer->stop();
        currentState = End;
        emit stateFinished();
    }
    else {
        gameTimer->stop();
        ui->TimerLabel->setText("00:00");
        //currentState = static_cast<state>(static_cast<int>(currentState)+1);
        emit timerFinished();
    }
}

void memory::advanceNum()
{
    if (index < NUM_NUM) {
        displayNum = seenNum[index++];
        ui->NumLabel->setText(QString::number(displayNum));
        countdownSeconds = DISPLAY_TIME;
        gameTimer->start(TIMER_INTERVAL);
        measurement->logEvent(MeasurementModule::Display, ACESBLUE::blue);
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
            ui->label->setText("Study the numbers below.");
            countdownSeconds = DISPLAY_TIME;
            index = 0;
            displayNum = seenNum[index];
            ui->NumLabel->setText(QString::number(displayNum));
            ui->NumLabel->show();
            //measurement->logEvent(MeasurementModule::Display, ACESBLUE::blue);
            gameTimer->start(TIMER_INTERVAL);
            index++;
            break;
        case Hide:
            countdownSeconds = DOWN_TIME;
            ui->TimerLabel->show();
            ui->TimerLabel->setText("00:30");
            gameTimer->start(TIMER_INTERVAL);
            ui->NumLabel->hide();
            ui->label->setText("Have a short break :)\nTry to list the alphabet backwards while you wait.");
            measurement->logEvent(MeasurementModule::MemBreak, ACESBLUE::blue);
            break;
        case Test:
            countdownSeconds = TEST_TIME;
            ui->label->setText("Is the number new or have you seen it before?");
            gameTimer->start(TIMER_INTERVAL);
            //gameTimer->start(5000);   // Used for accuracy testing, gives longer duration
            measurement->startCount();
            measurement->logEvent(MeasurementModule::Display, ACESBLUE::blue);
            ui->diffButtonFrame->show();
            index = 0;
            combineVectors(seenNum, newNum);
            displayNum = testNum[index];
            ui->NumLabel->setText(QString::number(displayNum));
            ui->NumLabel->show();
            index++;
            break;
        case End:
            // Display endgame stuff
            // Reset cycle
            QString info = "You got " + QString::number(correct)
                           + " out of " + QString::number(NUM_NUM);

            ui->TimerLabel->hide();
            ui->label->setText(info);
            currentState = Display;
            ui->diffButtonFrame->hide();
            ui->NumLabel->hide();
            ui->homeButton->show();
            measurement->logEvent(MeasurementModule::MemEnd, ACESBLUE::blue);
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
    correct = 0;
    measurement->logEvent(MeasurementModule::eventType::MemStart, ACESBLUE::blue);

    // Remove old instance of the ready form
    ui->stackedWidget->removeWidget(form);
    delete form;

    advanceGame();  // Advances to initial display case
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
        //qDebug() << testNum[index];
        //qDebug() << "New?: " << Ans[index];
        //qDebug() << "Selection: " << selection;

        if (selection == Ans[index-1]) {   // Compare user guess to actual value
        userAns[index] = 1;     // User made correct selection
        correct++;
        //qDebug() << "correct\n";
        measurement->logEvent(MeasurementModule::eventType::Correct, ACESBLUE::blue);
        }
        else {
        userAns[index-1] = 0;
        //qDebug() << "incorrect\n";
        measurement->logEvent(MeasurementModule::eventType::Incorrect, ACESBLUE::blue);
        }

        // Update number display
        displayNum = testNum[index++];
        ui->NumLabel->setText(QString::number(displayNum));
        measurement->logEvent(MeasurementModule::eventType::Display, ACESBLUE::blue);
    }
    else {
        if (selection == Ans[index-1]) {   // Compare user guess to actual value
        userAns[index] = 1;     // User made correct selection
        correct++;
        //qDebug() << "correct\n";
        measurement->logEvent(MeasurementModule::eventType::Correct, ACESBLUE::blue);
        }
        else {
        userAns[index-1] = 0;
        //qDebug() << "incorrect\n";
        measurement->logEvent(MeasurementModule::eventType::Incorrect, ACESBLUE::blue);
        }

        currentState = End;
        gameTimer->stop();
        emit stateFinished();
    }
}




