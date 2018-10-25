#ifndef DISPARITYDIALOG_H
#define DISPARITYDIALOG_H

#include <QDialog>
#include <QGraphicsPixmapItem>

namespace Ui {
class DisparityDialog;
}

class DisparityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisparityDialog(QWidget *parent = 0);
    ~DisparityDialog();
    void setPixmap(const QPixmap& disparity);

private:
    Ui::DisparityDialog *ui;
    QGraphicsPixmapItem *disparity_pixmap;
};

#endif // DISPARITYDIALOG_H
