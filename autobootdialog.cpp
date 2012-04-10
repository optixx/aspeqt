#include "autobootdialog.h"
#include "ui_autobootdialog.h"

#include <QTime>
#include <QDebug>

AutoBootDialog::AutoBootDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoBootDialog)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
}

AutoBootDialog::~AutoBootDialog()
{
    delete ui;
}

void AutoBootDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AutoBootDialog::booterStarted()
{
    ui->label->setText(tr("Atari is loading the booter."));
}

void AutoBootDialog::booterLoaded()
{
    ui->label->setText(tr("Atari is loading the program.\n\nFor some programs you may have to close this dialog manually when the program starts."));
    ui->progressBar->setVisible(true);
}

void AutoBootDialog::blockRead(int current, int all)
{
    ui->progressBar->setMaximum(all);
    ui->progressBar->setValue(current);
}

void AutoBootDialog::loaderDone()
{
    accept();
}
