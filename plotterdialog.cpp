#include "plotterdialog.h"
#include "ui_plotterdialog.h"

PlotterDialog::PlotterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotterDialog)
{
    ui->setupUi(this);
    graph = ui->graphicsView->addGraph();
}

void PlotterDialog::setDisparityPlot(QVector<int> cost)
{
    graph->clearData();
    int i=0;
    for(const auto& c : cost)
        graph->addData(i++,c);
    ui->graphicsView->rescaleAxes(true);
    ui->graphicsView->replot();
    setWindowTitle(QString("matching cost"));
}

PlotterDialog::~PlotterDialog()
{
    delete ui;
}
