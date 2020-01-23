#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QSerialPort>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Hildegart QT");
    w.show();
    return a.exec();
}

