#ifndef HTV_MAINWINDOW_H
#define HTV_MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
    class C_MainWindow;
}

class C_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit C_MainWindow(QWidget *parent = 0);
    ~C_MainWindow();

private:
    Ui::C_MainWindow *ui;
};

#endif // HTV_MAINWINDOW_H
