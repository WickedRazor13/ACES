#ifndef MEMORY_H
#define MEMORY_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QRandomGenerator>
#include <QTimer>
#include <algorithm>
#include <chrono>
#include "readyForm.h"
#include "measurementModule.h"

const int NUM_NUM = 48;

const int DISPLAY_TIME = 2; // To acheive 3 seconds, 2 is used for processing logic time
const int DOWN_TIME = 30;
const int TEST_TIME = DISPLAY_TIME * NUM_NUM;

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
        Test,
        End
    };
    Q_ENUM(state)

    explicit memory(QWidget *parent = nullptr);
    ~memory();

    void showForm();

    void initGame();

    void combineVectors(QVector<int>& vec1, QVector<int>& vec2);

private:
    Ui::memory *ui;

    MeasurementModule *measurement;

    ReadyForm *form;
    QTimer *gameTimer;
    int countdownSeconds;

    int displayNum;
    QString userNum;

    int correct = 0;
    int index = 0;

    QVector<int> seenNum = {468, 300, 666, 200, 333, 244,
                            313, 455, 202, 101, 332, 505,
                            808, 700, 680, 900, 880, 100,
                            111, 428, 720, 369, 360, 311,
                            537, 579, 748, 385, 317, 472,
                            147, 792, 816, 368, 796, 928,
                            847, 995, 458, 692, 752, 526,
                            852, 299, 671, 482, 713, 726};

    QVector<int> newNum = {913, 212, 424, 484, 442, 144,
                           707, 800, 699, 600, 242, 789,
                           248, 633, 744, 525, 123, 770,
                           464, 444, 363, 0, 120, 999,
                           536, 641, 463, 425, 964, 421,
                           418, 976, 719, 524, 863, 829,
                           649, 448, 773, 129, 921, 169,
                           724, 519, 697, 483, 869, 819};

    QVector<int> testNum;
    bool userAns[NUM_NUM];
    bool Ans[NUM_NUM];
    bool selection; // 0 = old, 1 = new

    state currentState;

signals:
    void timerFinished();
    void stateFinished();
    void homeClicked();
    void newNumDisplayed();
    void selectionMade();

private slots:
    void on_homeButton_clicked();
    void StartGame();
    void endGame();
    void exitGame();
    void advanceTimer();
    void advanceNum();
    void advanceGame();
    void on_oldButton_clicked();
    void on_newButton_clicked();
    void processSelection();
};

#endif // MEMORY_H
