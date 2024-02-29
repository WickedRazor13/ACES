#ifndef MEMORY_H
#define MEMORY_H

#include <QWidget>
#include <QString>
#include <QRandomGenerator>
#include <QTimer>
#include "readyForm.h"

const int NUM_NUM = 48;

const int DISPLAY_TIME = 10;
const int DOWN_TIME = 2;
const int ENTRY_TIME = 20;

const int RNG_LENGTH = 5;
const int RNG_MIN = 10000;
const int RNG_MAX = 99999;

namespace Ui {
class memory;
}

class memory : public QWidget
{
    Q_OBJECT

public:

    enum state {
        Display = 0,
        Hide,
        Receive,
        StateCount
    };
    Q_ENUM(state)

    explicit memory(QWidget *parent = nullptr);
    ~memory();

    void initGame();

    void compareNums(const QString& str1, const QString& str2);



private:
    Ui::memory *ui;

    QTimer *gameTimer;
    int countdownSeconds;

    int displayNum;
    QString userNum;

    int correct;

    int seenNum[NUM_NUM];
    int newNum[NUM_NUM];
    bool userAns[NUM_NUM];

    state currentState;

signals:
    void timerFinished();
    void homeClicked();

private slots:
    void on_homeButton_clicked();
    void endGame();
    void exitGame();
    void advanceTimerDisplay();
    void advanceGame();
    void on_lineEdit_returnPressed();
    void on_restartButton_clicked();
};

#endif // MEMORY_H
