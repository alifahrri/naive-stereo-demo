#include <QDebug>
#include <QApplication>
#include "mainwindow.h"
#include "stereovision.h"
#include "wrapper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    StereoVision stereo;

    wrapper::connect(&w,&stereo);
    wrapper::setStereoCallback(&stereo,&w);
    w.show();

    return a.exec();
}
