#ifndef ACES_H
#define ACES_H

#include <QWidget>
#include "spotTheDiff.h"
#include "memory.h"
#include "widget.h"
#include "bluetoothmanager.h"

// Window WxH
//const int WIDTH = 1300;
//const int HEIGHT = 720;

QT_BEGIN_NAMESPACE
namespace Ui { class ACES; }
QT_END_NAMESPACE

QT_BEGIN_INCLUDE_NAMESPACE
namespace ACESBLUE {
    extern BluetoothManager *blue;
}
QT_END_NAMESPACE

class ACES : public QWidget
{
    Q_OBJECT

public:
    ACES(QWidget *parent = nullptr);
    ~ACES();

    //BluetoothManager *blue;

private slots:
    void on_spotDiffButton_clicked();
    void moveHome();

    void on_quitButton_clicked();

    void on_memoryButton_clicked();

    void on_tetrisButton_clicked();

private:
    Ui::ACES *ui;

    SpotTheDiff *spotDiffGame;
    memory *MemoryGame;
    Widget *Tetrizz;


};
#endif // ACES_H
