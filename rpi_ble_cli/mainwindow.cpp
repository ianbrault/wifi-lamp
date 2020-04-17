/*
** mainwindow.cpp
*/

#include "deviceview.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setStyleSheet("background-color: #fff");
    setCentralWidget(new DeviceView());
}

MainWindow::~MainWindow()
{

}
