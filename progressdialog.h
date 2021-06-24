#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();
    void setTextBrowser(QString text);
    void appendText(QString text);
    void setProgressBarValue(int v);
    void setProgressBarRange(int min,int max);
    void setProgressBarTextVisible(bool b);
    bool getInterruptState();
    void setProgressThreadType(int progressThreadType);
    int getProgressThreadType();

signals:
    void interruptReceived();

private slots:
    void on_toolButton_clicked();

private:
    Ui::ProgressDialog *ui;

    //当前进度框关联线程的类型：1为复制粘贴线程，2为删除线程；用于对话框收到中断事件后判断终止哪个线程
    int progressThreadType=0;

    bool interrupt=false;     //中断标志，置为true后，会中断进度
};

#endif // PROGRESSDIALOG_H
