#include "smokewindow.h"
#include "ui_smokewindow.h"

SmokeWindow::SmokeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SmokeWindow)
{
    ui->setupUi(this);
}

SmokeWindow::~SmokeWindow()
{
    delete ui;
}
