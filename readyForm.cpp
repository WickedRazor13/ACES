#include "readyForm.h"
#include "ui_readyForm.h"

ReadyForm::ReadyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReadyForm)
{
    ui->setupUi(this);

    // Hide UI elements that are conditional
    ui->countdownFrame->hide();
    ui->countdownFrame_2->hide();
    ui->restartButton2->hide();

    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(ui->memoryStartButton, SIGNAL(clicked()), this, SLOT(on_memoryStartButton_clicked()));

    // Set up countdown timer
    countdownTimer = new QTimer(this);
    connect(countdownTimer, SIGNAL(timeout()), this, SLOT(updateCountdown()));

    ui->stackedWidget->setCurrentIndex(0);
}

ReadyForm::~ReadyForm()
{
    delete ui;
}

void ReadyForm::changeScreen(int index)
{
    this->ui->stackedWidget->setCurrentIndex(index);
}

void ReadyForm::changeEndLabel(QString msg)
{
    ui->endLabel->setText(msg);
}

void ReadyForm::changeResultLabel(QString msg)
{
    ui->resultsLabel->setText(msg);
}

void ReadyForm::showRestartOnGameEnd()
{
    ui->restartButton2->show();
}

bool ReadyForm::getRestartValue()
{
    return restart;
}

void ReadyForm::onStartButtonClicked()
{
    qDebug() << "Start button clicked";
    ui->countdownFrame->show();     // Show the countdown timer

    countdownValue = 3;
    countdownTimer->start(TIMER_INTERVAL);
    ui->countdownLabel->setText(QString::number(countdownValue));
    qDebug() << "Countdown started";
}

void ReadyForm::updateCountdown()
{
    // Decrease the countdown value
    countdownValue--;

    // Update the countdown labels
    ui->countdownLabel->setText(QString::number(countdownValue));
    ui->countdownLabel2->setText(QString::number(countdownValue));
    ui->countdownLabel_2->setText(QString::number(countdownValue));
    ui->resultsLabel->setText(QString::number(countdownValue));

    // If countdown is done, switch to the game screen
    if (countdownValue < 0) {
        countdownTimer->stop();
        if (memory == true) {
            emit startMemory();
            memory = false;
        }
        else {
            emit startGame();
        }
        qDebug() << "start game";
    }

}


void ReadyForm::on_restartButton_clicked()
{
    restart = true;

    countdownValue = 3;
    countdownTimer->start(TIMER_INTERVAL);
    ui->countdownLabel2->setText(QString::number(countdownValue));
    qDebug() << "Countdown started";
}


void ReadyForm::on_startNewButton_clicked()
{
    countdownValue = 3;
    countdownTimer->start(TIMER_INTERVAL);
    ui->countdownLabel2->setText(QString::number(countdownValue));
    qDebug() << "Countdown started";
}


void ReadyForm::on_exitButton_clicked()
{
    emit homeButtonClicked();
}


void ReadyForm::on_nextButton_clicked()
{
    countdownValue = 3;
    countdownTimer->start(TIMER_INTERVAL);
    QFont font;
    font.setPointSize(80);
    font.setBold(true);
    font.setItalic(true);
    ui->resultsLabel->setFont(font);
    ui->resultsLabel->setText(QString::number(countdownValue));
    qDebug() << "Countdown started";
}


void ReadyForm::on_restartButton2_clicked()
{
    restart = true;

    countdownValue = 3;
    countdownTimer->start(TIMER_INTERVAL);
    QFont font;
    font.setPointSize(80);
    font.setBold(true);
    font.setItalic(true);
    ui->resultsLabel->setFont(font);
    ui->resultsLabel->setText(QString::number(countdownValue));
    qDebug() << "Countdown started";
}


void ReadyForm::on_memoryStartButton_clicked()
{
    ui->countdownFrame_2->show();     // Show the countdown timer
    memory = true;

    countdownValue = 3;
    countdownTimer->start(TIMER_INTERVAL);
    ui->countdownLabel_2->setText(QString::number(countdownValue));
    qDebug() << "Countdown started";
}

