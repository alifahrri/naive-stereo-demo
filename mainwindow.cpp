#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "disparitydialog.h"
#include "plotterdialog.h"
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings_dialog(new SettingsDialog(this)),
    disparity_dialog(new DisparityDialog(this)),
    disparity_plotter(new PlotterDialog(this))
{
    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene(0,0,480*2+2,400,this));
    left = ui->graphicsView->scene()->addPixmap(QPixmap());
    right = ui->graphicsView->scene()->addPixmap(QPixmap());
    left->setPos(0,0);
    right->setPos(480+2,0);

    connect(ui->actionOpen,&QAction::triggered,[=](bool)
    {
        auto dir = QFileDialog::getExistingDirectory(this,QString("Open Directory"));
        QFile view1(dir+"/view1.png");
        QFile view5(dir+"/view5.png");
        qDebug() << "directory selected :" << dir;
        qDebug() << "view1 :" << (view1.exists() ? "exist" : "null");
        qDebug() << "view5 :" << (view5.exists() ? "exist" : "null");
        if(view1.exists() && view5.exists())
            emit setFiles(view1.fileName(), view5.fileName());
    });

    connect(this,&MainWindow::setPixmap,[=](const QPixmap& left_px, const QPixmap& right_px)
    {
        setImage(left_px,right_px);
    });

//    connect(this,&MainWindow::setDisparity,[=](const QPixmap& disparity)
//    {
//        this->disparity_dialog->setPixmap(disparity);
//    });

    connect(ui->actionSettings,&QAction::triggered,[=](bool)
    {
        settings_dialog->show();
    });

    connect(ui->actionCompute,&QAction::triggered,[=](bool)
    {
        emit disparityComputeRequest();
    });

    connect(ui->graphicsView,&GraphicsView::mousePress,[=](QPoint p)
    {
        emit matchCostRequest(p);
    });

    connect(settings_dialog,&QDialog::finished,[=](int result)
    {
        if(result)
        {
            auto disp = settings_dialog->maxDisparity();
            auto neighbor = settings_dialog->neighbor();
            auto step = settings_dialog->step();
            auto match = settings_dialog->match();
            emit setSADParameter(disp,neighbor,step,match);
        }
    });

    setCentralWidget(ui->graphicsView);
    setWindowTitle(QString("Stereo Test"));
    setFixedSize(ui->graphicsView->scene()->width()+50,
                 ui->graphicsView->scene()->height()+70);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDisparity(const QPixmap &disparity)
{
    this->disparity_dialog->setPixmap(disparity);
}

void MainWindow::setMatchCost(QVector<int> cost)
{
    this->disparity_plotter->setDisparityPlot(cost);
    disparity_plotter->show();
}

void MainWindow::setImage(const QPixmap &left_, const QPixmap &right_)
{
    ui->graphicsView->scene()->setSceneRect(0,0,left_.width()*2+3,left_.height());
    left->setPos(0,0);
    right->setPos(left_.width()+2,0);
    left->setPixmap(left_);
    right->setPixmap(right_);
}
