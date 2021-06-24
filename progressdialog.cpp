#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);

    this->setFixedSize(this->frameGeometry().size());
    this->setWindowFlags(this->windowFlags()|Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);

}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setTextBrowser(QString text)
{
    ui->textBrowser->setText(text);
}

void ProgressDialog::setProgressBarValue(int v)
{
    ui->progressBar->setValue(v);
}

void ProgressDialog::setProgressBarRange(int min, int max)
{
    ui->progressBar->setRange(min,max);
}

void ProgressDialog::setProgressBarTextVisible(bool b)
{
    ui->progressBar->setTextVisible(b);
}

void ProgressDialog::on_toolButton_clicked()
{
    interrupt=true;
    emit interruptReceived();
}

bool ProgressDialog::getInterruptState()
{
    return interrupt;
}

void ProgressDialog::appendText(QString text)
{
    ui->textBrowser->append(text);
}

void ProgressDialog::setProgressThreadType(int progressThreadType)
{
    this->progressThreadType=progressThreadType;
}

int ProgressDialog::getProgressThreadType()
{
    return progressThreadType;
}
