#ifndef AUTOBOOTDIALOG_H
#define AUTOBOOTDIALOG_H

#include <QDialog>

namespace Ui {
    class AutoBootDialog;
}

class AutoBootDialog : public QDialog {
    Q_OBJECT

public:
    AutoBootDialog(QWidget *parent = 0);
    ~AutoBootDialog();

// Add keep open signal // Ray A.
signals:
    void keepOpen();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AutoBootDialog *ui;

private slots:
    void booterStarted();
    void booterLoaded();
    void blockRead(int current, int all);
    void loaderDone();
};

#endif // AUTOBOOTDIALOG_H
