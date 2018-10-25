#ifndef PLOTTERDIALOG_H
#define PLOTTERDIALOG_H

#include <QDialog>
#include <QVector>
#include <QPair>
#include "qcustomplot.h"

namespace Ui {
class PlotterDialog;
}

class PlotterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotterDialog(QWidget *parent = 0);
    void setDisparityPlot(QVector<int> cost);
    ~PlotterDialog();

private:
    Ui::PlotterDialog *ui;
    QCPGraph* graph;
};

#endif // PLOTTERDIALOG_H
