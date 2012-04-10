#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QTranslator>
#include <QSystemTrayIcon>

#include "optionsdialog.h"
#include "aboutdialog.h"
#include "createimagedialog.h"
#include "diskeditdialog.h"

#include "serialport.h"
#include "sioworker.h"
#include "textprinterwindow.h"
// Ray A.
#include "docdisplaywindow.h"

namespace Ui
{
    class MainWindow;
}

class DiskWidgets
{
public:
    QLabel *fileNameLabel;
    QLabel *imagePropertiesLabel;
    QAction *saveAction;
    QAction *autoSaveAction;        // Ray A.
    QAction *saveAsAction;
    QAction *revertAction;
    QAction *mountDiskAction;
    QAction *mountFolderAction;
    QAction *ejectAction;
    QAction *writeProtectAction;
    QAction *editAction;
    QFrame *frame;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void show();

private:
    int untitledName;
    Ui::MainWindow *ui;
    SioWorker *sio;
    bool shownFirstTime;
    DiskWidgets diskWidgets[8];
    QLabel *speedLabel, *onOffLabel, *clearMessagesLabel;
    TextPrinterWindow *textPrinterWindow;
    // Ray A.
    DocDisplayWindow *docDisplayWindow;
    QTranslator aspeqt_translator, aspeqt_qt_translator;
    QSystemTrayIcon trayIcon;
    Qt::WindowFlags oldWindowFlags;
    Qt::WindowStates oldWindowStates;
    QString lastMessage;
    int lastMessageRepeat;
    
    void updateRecentFileActions();
    int firstEmptyDiskSlot(int startFrom = 0, bool createOne = true);
    int containingDiskSlot(const QPoint &point);
    void bootExe(const QString &fileName);
    void mountFile(int no, const QString &fileName, bool prot);
    void mountFileWithDefaultProtection(int no, const QString &fileName);
    void mountDiskImage(int no);
    void mountFolderImage(int no);
    bool ejectImage(int no, bool ask = true);
    void toggleWriteProtection(int no);
    void openEditor(int no);
    void saveDisk(int no);
    void autoSaveDisk(int no);  // Ray A.
    void saveDiskAs(int no);
    void revertDisk(int no);
    QMessageBox::StandardButton saveImageWhenClosing(int no, QMessageBox::StandardButton previousAnswer, int number);
    void loadTranslators();

protected:
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);
    void hideEvent(QHideEvent *event);

signals:
    void logMessage(int type, const QString &msg);

public:
    void doLogMessage(int type, const QString &msg);

private slots:
    void on_actionPlaybackCassette_triggered();
    void on_actionShowPrinterTextOutput_triggered();
    void on_actionBootExe_triggered();
    void on_actionSaveSession_triggered();
    void on_actionOpenSession_triggered();
    void on_actionNewImage_triggered();
    void on_actionMountFolder_triggered();
    void on_actionMountDisk_triggered();
    void on_actionEjectAll_triggered();
    void on_actionOptions_triggered();
    void on_actionStartEmulation_triggered();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    // Ray A.
    void on_actionDocumentation_triggered();

    void on_actionMountDisk_1_triggered();
    void on_actionMountDisk_2_triggered();
    void on_actionMountDisk_3_triggered();
    void on_actionMountDisk_4_triggered();
    void on_actionMountDisk_5_triggered();
    void on_actionMountDisk_6_triggered();
    void on_actionMountDisk_7_triggered();
    void on_actionMountDisk_8_triggered();

    void on_actionMountFolder_1_triggered();
    void on_actionMountFolder_2_triggered();
    void on_actionMountFolder_3_triggered();
    void on_actionMountFolder_4_triggered();
    void on_actionMountFolder_5_triggered();
    void on_actionMountFolder_6_triggered();
    void on_actionMountFolder_7_triggered();
    void on_actionMountFolder_8_triggered();

    void on_actionEject_1_triggered();
    void on_actionEject_2_triggered();
    void on_actionEject_3_triggered();
    void on_actionEject_4_triggered();
    void on_actionEject_5_triggered();
    void on_actionEject_6_triggered();
    void on_actionEject_7_triggered();
    void on_actionEject_8_triggered();

    void on_actionWriteProtect_1_triggered();
    void on_actionWriteProtect_2_triggered();
    void on_actionWriteProtect_3_triggered();
    void on_actionWriteProtect_4_triggered();
    void on_actionWriteProtect_5_triggered();
    void on_actionWriteProtect_6_triggered();
    void on_actionWriteProtect_7_triggered();
    void on_actionWriteProtect_8_triggered();

    void on_actionMountRecent_0_triggered();
    void on_actionMountRecent_1_triggered();
    void on_actionMountRecent_2_triggered();
    void on_actionMountRecent_3_triggered();
    void on_actionMountRecent_4_triggered();
    void on_actionMountRecent_5_triggered();
    void on_actionMountRecent_6_triggered();
    void on_actionMountRecent_7_triggered();
    void on_actionMountRecent_8_triggered();
    void on_actionMountRecent_9_triggered();

    void on_actionEditDisk_1_triggered();
    void on_actionEditDisk_2_triggered();
    void on_actionEditDisk_3_triggered();
    void on_actionEditDisk_4_triggered();
    void on_actionEditDisk_5_triggered();
    void on_actionEditDisk_6_triggered();
    void on_actionEditDisk_7_triggered();
    void on_actionEditDisk_8_triggered();

    void on_actionSave_1_triggered();
    void on_actionSave_2_triggered();
    void on_actionSave_3_triggered();
    void on_actionSave_4_triggered();
    void on_actionSave_5_triggered();
    void on_actionSave_6_triggered();
    void on_actionSave_7_triggered();
    void on_actionSave_8_triggered();
// Ray A.
    void on_actionAutoSave_1_triggered();
    void on_actionAutoSave_2_triggered();
    void on_actionAutoSave_3_triggered();
    void on_actionAutoSave_4_triggered();
    void on_actionAutoSave_5_triggered();
    void on_actionAutoSave_6_triggered();
    void on_actionAutoSave_7_triggered();
    void on_actionAutoSave_8_triggered();
//
    void on_actionSaveAs_1_triggered();
    void on_actionSaveAs_2_triggered();
    void on_actionSaveAs_3_triggered();
    void on_actionSaveAs_4_triggered();
    void on_actionSaveAs_5_triggered();
    void on_actionSaveAs_6_triggered();
    void on_actionSaveAs_7_triggered();
    void on_actionSaveAs_8_triggered();

    void on_actionRevert_1_triggered();
    void on_actionRevert_2_triggered();
    void on_actionRevert_3_triggered();
    void on_actionRevert_4_triggered();
    void on_actionRevert_5_triggered();
    void on_actionRevert_6_triggered();
    void on_actionRevert_7_triggered();
    void on_actionRevert_8_triggered();

    void sioFinished();
    void sioStarted();
    void sioStatusChanged(QString status);
    void textPrinterWindowClosed();
    // Ray A.
    void docDisplayWindowClosed();
    void deviceStatusChanged(int deviceNo);
    void uiMessage(int t, const QString message);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // MAINWINDOW_H
