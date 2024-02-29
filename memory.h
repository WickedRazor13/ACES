#ifndef MEMORY_H
#define MEMORY_H

#include <QWidget>

namespace Ui {
class memory;
}

class memory : public QWidget
{
    Q_OBJECT

public:
    explicit memory(QWidget *parent = nullptr);
    ~memory();

private:
    Ui::memory *ui;
};

#endif // MEMORY_H
