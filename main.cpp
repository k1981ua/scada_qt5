#include "mainwindow.h"
#include <QApplication>
#include "usercode.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    UserCode usercode;
    MainWindow w;

    w.show();

    return a.exec();
}
