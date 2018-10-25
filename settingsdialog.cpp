#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setFixedSize(this->size());
    ui->comboBox->addItem(QString("CENSUS"));
    ui->comboBox->addItem(QString("SAD"));
    setWindowTitle(QString("SAD Parameter"));
}

int SettingsDialog::neighbor()
{
    return ui->window_sbox->value();
}

int SettingsDialog::step()
{
    return ui->step_sbox->value();
}

QString SettingsDialog::match()
{
    return ui->comboBox->currentText();
}

int SettingsDialog::maxDisparity()
{
    return ui->disp_sbox->value();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
