#ifndef WRAPPER_H
#define WRAPPER_H

#include "mainwindow.h"
#include "stereovision.h"
#include <QDebug>

namespace wrapper {

inline
void connect(MainWindow *w, StereoVision *stereo)
{
    QObject::connect(w,&MainWindow::setFiles,[=](QString left, QString right)
    {
        if(stereo->setFiles(left.toStdString(),right.toStdString()))
            qDebug() << "open files ok";
        else
            qDebug() << "open files failed";
        auto disparity = stereo->disparity();
//        cv::imshow("disparity",disparity);
        auto disparity_pxmap = QPixmap::fromImage(QImage((unsigned char*) disparity.data,
                                                     disparity.cols,
                                                     disparity.rows,
                                                     disparity.step,QImage::Format_Grayscale8).rgbSwapped());
        w->setDisparity(disparity_pxmap);
    });

    QObject::connect(w,&MainWindow::setSADParameter,[=](int disparity, int neighbor, int step, QString match)
    {
        stereo->setMaxDisparity(disparity);
        stereo->setWindow(neighbor);
        stereo->setDisparityStep(step);
        stereo->setMatchingMethod(match.toStdString());
    });

    QObject::connect(w,&MainWindow::disparityComputeRequest,[=]{
        auto disparity = stereo->disparity();
//        cv::imshow("disparity",disparity);
        auto disparity_pxmap = QPixmap::fromImage(QImage((unsigned char*) disparity.data,
                                                     disparity.cols,
                                                     disparity.rows,
                                                     disparity.step,QImage::Format_Grayscale8).rgbSwapped());
        w->setDisparity(disparity_pxmap);
    });

    QObject::connect(w,&MainWindow::matchCostRequest,[=](QPoint point)
    {
        auto match = stereo->matchCost(point.x(),point.y());
        auto qmatch = QVector<int>::fromStdVector(match);
        w->setMatchCost(qmatch);
    });

}

inline
void setStereoCallback(StereoVision *stereo, MainWindow *w)
{
    stereo->connectImageCallback([=](const cv::Mat& left, const cv::Mat& right)
    {
        qDebug() << "image callback";
        auto left_pxmap = QPixmap::fromImage(QImage((unsigned char*) left.data,
                                                    left.cols,
                                                    left.rows,
                                                    left.step,QImage::Format_RGB888).rgbSwapped());
        auto right_pxmap = QPixmap::fromImage(QImage((unsigned char*) right.data,
                                                     right.cols,
                                                     right.rows,
                                                     right.step,QImage::Format_RGB888).rgbSwapped());
        w->setPixmap(left_pxmap,right_pxmap);
    });
}

}

#endif // WRAPPER_H
