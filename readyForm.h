#ifndef READYFORM_H
#define READYFORM_H

#include <QWidget>
#include <QTimer>

const int TIMER_INTERVAL = 1000;

namespace Ui {
class ReadyForm;
}

class ReadyForm : public QWidget
{
    Q_OBJECT

public:
    explicit ReadyForm(QWidget *parent = nullptr);
    ~ReadyForm();

    void changeScreen(int index);
    void changeEndLabel(QString msg);
    void changeResultLabel(QString msg);
    void showRestartOnGameEnd();
    bool getRestartValue();

signals:
    void startGame();
    void homeButtonClicked();

private slots:
    void onStartButtonClicked();
    void updateCountdown();

    void on_restartButton_clicked();

    void on_startNewButton_clicked();

    void on_exitButton_clicked();

    void on_nextButton_clicked();

    void on_restartButton2_clicked();

private:
    Ui::ReadyForm *ui;

    QTimer *countdownTimer;
    int countdownValue;

    bool restart = false;
};

#endif // READYFORM_H
