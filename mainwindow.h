#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QGraphicsPixmapItem;
class SettingsDialog;
class DisparityDialog;
class PlotterDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setDisparity(const QPixmap& disparity);
    void setMatchCost(QVector<int> cost);

private:
    void setImage(const QPixmap &left_, const QPixmap &right_);

private:
    Ui::MainWindow *ui;
    SettingsDialog *settings_dialog;
    DisparityDialog *disparity_dialog;
    PlotterDialog *disparity_plotter;
    QGraphicsPixmapItem *left;
    QGraphicsPixmapItem *right;

signals:
    void disparityComputeRequest();
    void matchCostRequest(QPoint);
    void setFiles(QString left, QString right);
    void setPixmap(const QPixmap& left_px, const QPixmap& right_px);
    void setSADParameter(int disparity, int neighbor, int step, QString match);
};

#endif // MAINWINDOW_H
