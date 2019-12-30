#include "cnc_automation.h"

#include <QApplication>


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    CNC_automation w;
    return a.exec();
}
