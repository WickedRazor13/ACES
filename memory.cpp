#include "memory.h"
#include "ui_memory.h"

memory::memory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::memory)
{
    ui->setupUi(this);
}

memory::~memory()
{
    delete ui;
}
