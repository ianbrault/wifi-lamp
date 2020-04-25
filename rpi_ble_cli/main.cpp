/*
** main.cpp
*/

#include <QApplication>
#include <QMainWindow>

#include "deviceview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QMainWindow w;
    w.setStyleSheet("background-color: #fff");
    w.setCentralWidget(new DeviceView());
    w.show();

    return a.exec();
}
