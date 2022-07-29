#include "mapgeneratorapp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    MapGeneratorApp appWindow;
    appWindow.show();

    return application.exec();
}
