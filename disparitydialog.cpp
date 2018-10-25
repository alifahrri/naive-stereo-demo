#include "disparitydialog.h"
#include "ui_disparitydialog.h"

DisparityDialog::DisparityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisparityDialog)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene(0,0,500,500,this));
    disparity_pixmap = ui->graphicsView->scene()->addPixmap(QPixmap());
    setWindowTitle(QString("disparity"));
}

DisparityDialog::~DisparityDialog()
{
    delete ui;
}

void DisparityDialog::setPixmap(const QPixmap &disparity)
{
    this->ui->graphicsView->scene()->setSceneRect(0,0,disparity.width(),disparity.height());
    this->setFixedSize(disparity.width()+5,disparity.height()+5);
    disparity_pixmap->setPixmap(disparity);
    this->show();
}
