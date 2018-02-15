#include "htv_mainwindow.h"
#include "ui_htv_mainwindow.h"

C_MainWindow::C_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::C_MainWindow)
{
    ui->setupUi(this);
}

C_MainWindow::~C_MainWindow()
{
    delete ui;
}
