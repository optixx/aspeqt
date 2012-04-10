#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "diskimage.h"
#include "diskimagepro.h"
#include "diskimageatx.h"
#include "folderimage.h"
#include "miscdevices.h"
#include "aspeqtsettings.h"
#include "autobootdialog.h"
#include "autoboot.h"
#include "cassettedialog.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QScrollBar>
#include <QTranslator>

#include "atarifilesystem.h"
#include "miscutils.h"

AspeqtSettings *aspeqtSettings;
MainWindow *mainWindow;

QFile *logFile;
QMutex *logMutex;

// Displayed only in debug mode    "!d"
// Unimportant     (gray)          "!u"
// Normal          (black)         "!n"
// Important       (blue)          "!i"
// Warning         (brown)         "!w"
// Error           (red)           "!e"

void logMessageOutput(QtMsgType type, const char *msg)
{
    logMutex->lock();
    logFile->write(QString::number((quint64)QThread::currentThreadId(), 16).toAscii());
    switch (type) {
        case QtDebugMsg:
            logFile->write(": [Debug]    ");
            break;
        case QtWarningMsg:
            logFile->write(": [Warning]  ");
            break;
        case QtCriticalMsg:
            logFile->write(": [Critical] ");
            break;
        case QtFatalMsg:
            logFile->write(": [Fatal]    ");
            break;
    }
    logFile->write(msg);
    logFile->write("\n");
    if (type == QtFatalMsg) {
        logFile->close();
        abort();
    }
    logMutex->unlock();

    if (msg[0] == '!') {
#ifdef QT_NO_DEBUG
        if (msg[1] == 'd') {
            return;
        }
#endif
        mainWindow->doLogMessage(msg[1], QString::fromLocal8Bit(msg+3));
    }
}

void MainWindow::doLogMessage(int type, const QString &msg)
{
    emit logMessage(type, msg);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    /* Setup the logging system */
    mainWindow = this;
    logFile = new QFile(QDir::temp().absoluteFilePath("aspeqt.log"));
    logFile->open(QFile::WriteOnly | QFile::Truncate | QFile::Unbuffered | QFile::Text);
    logMutex = new QMutex();
    connect(this, SIGNAL(logMessage(int,QString)), this, SLOT(uiMessage(int,QString)), Qt::QueuedConnection);
    qInstallMsgHandler(logMessageOutput);
    qDebug() << "!d" << tr("AspeQt started at %1.").arg(QDateTime::currentDateTime().toString());

    /* Remove old temporaries */
    QDir tempDir = QDir::temp();
    QStringList filters;
    filters << "aspeqt-*";
    QFileInfoList list = tempDir.entryInfoList(filters, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    foreach(QFileInfo file, list) {
        deltree(file.absoluteFilePath());
    }
    
    /* Set application properties */
    QCoreApplication::setOrganizationName("TrayGun Software");
    QCoreApplication::setApplicationName("AspeQt");
    aspeqtSettings = new AspeqtSettings();
    
    /* Load translators */
    loadTranslators();
    
    /* Setup UI */
    ui->setupUi(this);

    // Restore MainWindow to last saved positions and size // Ray A.
    mainWindow->setGeometry(aspeqtSettings->lastHorizontalPos(),aspeqtSettings->lastVerticalPos(),aspeqtSettings->lastWidth(),aspeqtSettings->lastHeight());

    /* Setup status bar */
    speedLabel = new QLabel(this);
    onOffLabel = new QLabel(this);
    clearMessagesLabel = new QLabel(this);
    speedLabel->setText(tr("19200 bits/sec"));
    onOffLabel->setMinimumWidth(21);

    clearMessagesLabel->setMinimumWidth(21);
    clearMessagesLabel->setPixmap(QIcon(":/icons/silk-icons/icons/page_white.png").pixmap(16, 16, QIcon::Normal));
    clearMessagesLabel->setToolTip(tr("Clear messages"));
    clearMessagesLabel->setStatusTip(clearMessagesLabel->toolTip());

    speedLabel->setMinimumWidth(80);

    ui->statusBar->addPermanentWidget(speedLabel);
    ui->statusBar->addPermanentWidget(onOffLabel);
    ui->statusBar->addPermanentWidget(clearMessagesLabel);

    /* Initialize diskWidgets array and tool button actions */
    
    for (int i = 0; i < 8; i++) {
        diskWidgets[i].fileNameLabel = findChild <QLabel*> (QString("labelFileName_%1").arg(i + 1));
        diskWidgets[i].imagePropertiesLabel = findChild <QLabel*> (QString("labelImageProperties_%1").arg(i + 1));
        diskWidgets[i].ejectAction = findChild <QAction*> (QString("actionEject_%1").arg(i + 1));
        diskWidgets[i].writeProtectAction = findChild <QAction*> (QString("actionWriteProtect_%1").arg(i + 1));
        diskWidgets[i].editAction = findChild <QAction*> (QString("actionEditDisk_%1").arg(i + 1));
        diskWidgets[i].mountDiskAction = findChild <QAction*> (QString("actionMountDisk_%1").arg(i + 1));
        diskWidgets[i].mountFolderAction = findChild <QAction*> (QString("actionMountFolder_%1").arg(i + 1));
        diskWidgets[i].saveAction = findChild <QAction*> (QString("actionSave_%1").arg(i + 1));
        diskWidgets[i].autoSaveAction = findChild <QAction*> (QString("actionAutoSave_%1").arg(i + 1));  // Ray A.
        diskWidgets[i].revertAction = findChild <QAction*> (QString("actionRevert_%1").arg(i + 1));
        diskWidgets[i].saveAsAction = findChild <QAction*> (QString("actionSaveAs_%1").arg(i + 1));
        diskWidgets[i].frame = findChild <QFrame*> (QString("horizontalFrame_%1").arg(i + 1));

        diskWidgets[i].frame->insertAction(0, diskWidgets[i].saveAction);
        diskWidgets[i].frame->insertAction(0, diskWidgets[i].autoSaveAction);   // Ray A.
        diskWidgets[i].frame->insertAction(0, diskWidgets[i].saveAsAction);
        diskWidgets[i].frame->insertAction(0, diskWidgets[i].revertAction);
        diskWidgets[i].frame->insertAction(0, diskWidgets[i].mountDiskAction);
        diskWidgets[i].frame->insertAction(0, diskWidgets[i].mountFolderAction);
        diskWidgets[i].frame->insertAction(0, diskWidgets[i].ejectAction);
        diskWidgets[i].frame->insertAction(0, diskWidgets[i].writeProtectAction);
        diskWidgets[i].frame->insertAction(0, diskWidgets[i].editAction);

        findChild <QToolButton*> (QString("buttonMountDisk_%1").arg(i + 1)) -> setDefaultAction(diskWidgets[i].mountDiskAction);
        findChild <QToolButton*> (QString("buttonMountFolder_%1").arg(i + 1)) -> setDefaultAction(diskWidgets[i].mountFolderAction);
        findChild <QToolButton*> (QString("buttonEject_%1").arg(i + 1)) -> setDefaultAction(diskWidgets[i].ejectAction);
        findChild <QToolButton*> (QString("buttonSave_%1").arg(i + 1)) -> setDefaultAction(diskWidgets[i].saveAction);
        findChild <QToolButton*> (QString("autoSave_%1").arg(i + 1)) -> setDefaultAction(diskWidgets[i].autoSaveAction);  // Ray A.
        findChild <QToolButton*> (QString("buttonEditDisk_%1").arg(i + 1)) -> setDefaultAction(diskWidgets[i].editAction);
    }

    /* Connect SioWorker signals */
    sio = new SioWorker();
    connect(sio, SIGNAL(started()), this, SLOT(sioStarted()));
    connect(sio, SIGNAL(finished()), this, SLOT(sioFinished()));
    connect(sio, SIGNAL(statusChanged(QString)), this, SLOT(sioStatusChanged(QString)));
    shownFirstTime = true;

    /* Restore application state */
    for (int i = 0; i < 8; i++) {
        AspeqtSettings::ImageSettings is;
        is = aspeqtSettings->mountedImageSetting(i);
        mountFile(i, is.fileName, is.isWriteProtected);
    }
    updateRecentFileActions();

    setAcceptDrops(true);

    ApeTime *ape = new ApeTime(sio);
    sio->installDevice(0x45, ape);

    // AspeQt Server  // Ray A.
    AspeCl *acl = new AspeCl(sio);
    sio->installDevice(0x46, acl);

    textPrinterWindow = new TextPrinterWindow();
    // Documentation Display  Ray A.
    docDisplayWindow = new DocDisplayWindow();

    connect(textPrinterWindow, SIGNAL(closed()), this, SLOT(textPrinterWindowClosed()));
    // Ray A.
    connect(docDisplayWindow, SIGNAL(closed()), this, SLOT(docDisplayWindowClosed()));

    Printer *printer = new Printer(sio);
    connect(printer, SIGNAL(print(QString)), textPrinterWindow, SLOT(print(QString)));
    sio->installDevice(0x40, printer);
    untitledName = 0;

    trayIcon.setIcon(windowIcon());
    connect(&trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
}

MainWindow::~MainWindow()
{
    if (ui->actionStartEmulation->isChecked()) {
        ui->actionStartEmulation->trigger();
    }

    delete aspeqtSettings;
    delete sio;

    delete ui;

    qDebug() << "!d" << tr("AspeQt stopped at %1.").arg(QDateTime::currentDateTime().toString());
    qInstallMsgHandler(0);
    delete logMutex;
    delete logFile;
}

 void MainWindow::mousePressEvent(QMouseEvent *event)
 {
     int slot = containingDiskSlot(event->pos());

     if (event->button() == Qt::LeftButton
         && slot >= 0) {

         QDrag *drag = new QDrag(this);
         QMimeData *mimeData = new QMimeData;

         mimeData->setData("application/x-aspeqt-disk-image", QByteArray(1, slot));
         drag->setMimeData(mimeData);

         drag->exec();
     }

     if (event->button() == Qt::LeftButton && onOffLabel->geometry().translated(ui->statusBar->geometry().topLeft()).contains(event->pos())) {
         ui->actionStartEmulation->trigger();
     }

      if (event->button() == Qt::LeftButton && clearMessagesLabel->geometry().translated(ui->statusBar->geometry().topLeft()).contains(event->pos())) {
         ui->textEdit->clear();
     }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    int i = containingDiskSlot(event->pos());
    if (i >= 0 && (event->mimeData()->hasUrls() ||
                   event->mimeData()->hasFormat("application/x-aspeqt-disk-image"))) {
        event->acceptProposedAction();
    } else {
        i = -1;
    }
    for (int j = 0; j <= 7; j++) {
        if (i == j) {
            diskWidgets[j].frame->setFrameShadow(QFrame::Sunken);
        } else {
            diskWidgets[j].frame->setFrameShadow(QFrame::Raised);
        }
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    int i = containingDiskSlot(event->pos());
    if (i >= 0 && (event->mimeData()->hasUrls() ||
                   event->mimeData()->hasFormat("application/x-aspeqt-disk-image"))) {
        event->acceptProposedAction();
    } else {
        i = -1;
    }
    for (int j = 0; j <= 7; j++) {
        if (i == j) {
            diskWidgets[j].frame->setFrameShadow(QFrame::Sunken);
        } else {
            diskWidgets[j].frame->setFrameShadow(QFrame::Raised);
        }
    }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *)
{
    for (int j = 0; j <= 7; j++) {
        diskWidgets[j].frame->setFrameShadow(QFrame::Raised);
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    for (int j = 0; j <= 7; j++) {
        diskWidgets[j].frame->setFrameShadow(QFrame::Raised);
    }
    int slot = containingDiskSlot(event->pos());
    if (!(event->mimeData()->hasUrls() ||
          event->mimeData()->hasFormat("application/x-aspeqt-disk-image")) ||
          slot < 0) {
        return;
    }

    if (event->mimeData()->hasFormat("application/x-aspeqt-disk-image")) {
        int source = event->mimeData()->data("application/x-aspeqt-disk-image").at(0);

        if (slot == source) {
            return;
        }

        sio->swapDevices(slot + 0x31, source + 0x31);

        aspeqtSettings->swapImages(slot, source);

        qDebug() << "!n" << tr("Swapped disk %1 with disk %2.").arg(slot + 1).arg(source + 1);

        return;
    }

    QStringList files;
    foreach (QUrl url, event->mimeData()->urls()) {
        if (!url.toLocalFile().isEmpty()) {
            files.append(url.toLocalFile());
        }
    }
    if (files.isEmpty()) {
        return;
    }

    FileTypes::FileType type = FileTypes::getFileType(files.at(0));

    if (type == FileTypes::Xex) {
        bootExe(files.at(0));
        return;
    }

    if (type == FileTypes::Cas) {
        bool restart;
        restart = ui->actionStartEmulation->isChecked();
        if (restart) {
            ui->actionStartEmulation->trigger();
            sio->wait();
            qApp->processEvents();
        }

        CassetteDialog *dlg = new CassetteDialog(this, files.at(0));
        dlg->exec();
        delete dlg;

        if (restart) {
            ui->actionStartEmulation->trigger();
        }
        return;
    }

    mountFileWithDefaultProtection(slot, files[0]);
    files.removeAt(0);
    while (!files.isEmpty() && (slot = firstEmptyDiskSlot(slot, false)) >= 0) {
        mountFileWithDefaultProtection(slot, files[0]);
        files.removeAt(0);
    }
    slot = 0;
    while (!files.isEmpty() && (slot = firstEmptyDiskSlot(slot, false)) >= 0) {
        mountFileWithDefaultProtection(slot, files[0]);
        files.removeAt(0);
    }
    foreach(QString file, files) {
        qCritical() << "!e" << tr("Cannot mount '%1': No empty disk slots.").arg(file);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save Current MainWindow Position and size // Ray A.
    aspeqtSettings->setLastHorizontalPos(mainWindow->geometry().x());
    aspeqtSettings->setLastVerticalPos(mainWindow->geometry().y());
    aspeqtSettings->setLastWidth(mainWindow->geometry().width());
    aspeqtSettings->setLastHeight(mainWindow->geometry().height());

    bool wasRunning = ui->actionStartEmulation->isChecked();
    QMessageBox::StandardButton answer = QMessageBox::No;

    if (wasRunning) {
        ui->actionStartEmulation->trigger();
    }

    int toBeSaved = 0;

    for (int i = 0; i < 8; i++) {
        SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(i + 0x31));
        if (img && img->isModified()) {
            toBeSaved++;
        }
    }

    for (int i = 0; i < 8; i++) {
        SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(i + 0x31));
        if (img && img->isModified()) {
            toBeSaved--;
            answer = saveImageWhenClosing(i, answer, toBeSaved);
            if (answer == QMessageBox::NoToAll) {
                break;
            }
            if (answer == QMessageBox::Cancel) {
                if (wasRunning) {
                    ui->actionStartEmulation->trigger();
                }
                event->ignore();
                return;
            }
        }
    }

    delete textPrinterWindow;
    // Ray A.
    delete docDisplayWindow;

    for (int i = 0x31; i < 0x39; i++) {
        SimpleDiskImage *s = qobject_cast <SimpleDiskImage*> (sio->getDevice(i));
        if (s) {
            s->close();
        }
    }

    event->accept();
}

void MainWindow::hideEvent(QHideEvent *event)
{
    if (aspeqtSettings->minimizeToTray()) {
        trayIcon.show();
        oldWindowFlags = windowFlags();
        oldWindowStates = windowState();
        setWindowFlags(Qt::Widget);
        hide();
        event->ignore();
        return;
    }
    QMainWindow::hideEvent(event);
}

void MainWindow::show()
{
    QMainWindow::show();
    if (shownFirstTime) {
        /* Open options dialog if it's the first time */
        if (aspeqtSettings->isFirstTime()) {
            if (QMessageBox::Yes == QMessageBox::question(this, tr("First run"),
                                       tr("You are running AspeQt for the first time.\n\nDo you want to open the options dialog?"),
                                       QMessageBox::Yes, QMessageBox::No)) {
                ui->actionOptions->trigger();
            }
        }
//        qDebug() << "!d" << "Starting emulation";     // Ray A.
        qDebug() << "!d" << tr("Starting emulation");

        ui->actionStartEmulation->trigger();
    }
}

void MainWindow::on_actionStartEmulation_triggered()
{
    if (ui->actionStartEmulation->isChecked()) {
        sio->start(QThread::TimeCriticalPriority);
    } else {
        sio->setPriority(QThread::NormalPriority);
        sio->wait();
        qApp->processEvents();
    }
}

void MainWindow::sioStarted()
{
    ui->actionStartEmulation->setText(tr("&Stop emulation"));
    ui->actionStartEmulation->setToolTip(tr("Stop SIO peripheral emulation"));
    ui->actionStartEmulation->setStatusTip(tr("Stop SIO peripheral emulation"));
    onOffLabel->setPixmap(ui->actionStartEmulation->icon().pixmap(16, QIcon::Normal, QIcon::On));
    onOffLabel->setToolTip(ui->actionStartEmulation->toolTip());
    onOffLabel->setStatusTip(ui->actionStartEmulation->statusTip());
}

void MainWindow::sioFinished()
{
    ui->actionStartEmulation->setText(tr("&Start emulation"));
    ui->actionStartEmulation->setToolTip(tr("Start SIO peripheral emulation"));
    ui->actionStartEmulation->setStatusTip(tr("Start SIO peripheral emulation"));
    ui->actionStartEmulation->setChecked(false);
    onOffLabel->setPixmap(ui->actionStartEmulation->icon().pixmap(16, QIcon::Normal, QIcon::Off));
    onOffLabel->setToolTip(ui->actionStartEmulation->toolTip());
    onOffLabel->setStatusTip(ui->actionStartEmulation->statusTip());
    speedLabel->hide();
    speedLabel->clear();
    qWarning() << "!i" << tr("Emulation stopped.");
}

void MainWindow::sioStatusChanged(QString status)
{
    speedLabel->setText(status);
    speedLabel->show();
}

void MainWindow::deviceStatusChanged(int deviceNo)
{
    if (deviceNo >= 0x31 && deviceNo <= 0x38) {
        SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(deviceNo));
        if (img) {

            // Show file name without the path and set toolTip & statusTip to show the path separately // Ray A.
            QString filenamelabel;
            int i;
            if (img->description() == tr("Folder image")) {
                i = img->originalFileName().lastIndexOf("\\");
            } else {
                i = img->originalFileName().lastIndexOf("/");
            }
               if (i == -1) {
                   i = img->originalFileName().lastIndexOf("/");
               }
            if ((i != -1) || (img->originalFileName().mid(0, 14) == "Untitled image")) {
                filenamelabel = img->originalFileName().right(img->originalFileName().size() - ++i);
            } else {
                filenamelabel = "!!!!!!!!.!!!";
                }

            findChild <QLabel*> (QString("labelFileName_%1").arg(deviceNo - 0x31 + 1))->setToolTip(img->originalFileName().left(i - 1));
            findChild <QLabel*> (QString("labelFileName_%1").arg(deviceNo - 0x31 + 1))->setStatusTip(img->originalFileName());

            // diskWidgets[deviceNo - 0x31].fileNameLabel->setText(img->originalFileName());
            diskWidgets[deviceNo - 0x31].fileNameLabel->setText(filenamelabel);
            diskWidgets[deviceNo - 0x31].imagePropertiesLabel->setText(img->description());
            diskWidgets[deviceNo - 0x31].ejectAction->setEnabled(true);
            diskWidgets[deviceNo - 0x31].editAction->setChecked(img->editDialog() != 0);
            if (img->description() == tr("Folder image")) {
                diskWidgets[deviceNo - 0x31].editAction->setEnabled(false);
                diskWidgets[deviceNo - 0x31].saveAsAction->setEnabled(false);
                diskWidgets[deviceNo - 0x31].saveAction->setEnabled(false);
                diskWidgets[deviceNo - 0x31].autoSaveAction->setEnabled(false);         // Ray A.
                diskWidgets[deviceNo - 0x31].revertAction->setEnabled(false);
            } else {
                diskWidgets[deviceNo - 0x31].editAction->setEnabled(true);
                diskWidgets[deviceNo - 0x31].saveAsAction->setEnabled(true);
                diskWidgets[deviceNo - 0x31].autoSaveAction->setEnabled(true);          // Ray A.

                if (img->isModified()) {
                    if (!diskWidgets[deviceNo - 0x31].autoSaveAction->isChecked()) {    // Ray A.
                        diskWidgets[deviceNo - 0x31].saveAction->setEnabled(true);
                        diskWidgets[deviceNo - 0x31].revertAction->setEnabled(true);
                    // Image is modified and autosave is checked, so save the image (no need to lock it)  // Ray A.
                    } else {
                        bool saved;
                        saved = img->save();
                        if (!saved) {
                            if (QMessageBox::question(this, tr("Save failed"), tr("'%1' cannot be saved, do you want to save the image with another name?")
                                .arg(img->originalFileName()), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
                                saveDiskAs(deviceNo);
                            }
                        } else {
                            diskWidgets[deviceNo - 0x31].saveAction->setEnabled(false);
                            diskWidgets[deviceNo - 0x31].revertAction->setEnabled(false);
                        }
                    }
                } else {
                    diskWidgets[deviceNo - 0x31].saveAction->setEnabled(false);
                    diskWidgets[deviceNo - 0x31].revertAction->setEnabled(false);
                }
            }
        } else {
            diskWidgets[deviceNo - 0x31].saveAction->setEnabled(false);
            diskWidgets[deviceNo - 0x31].fileNameLabel->clear();
            diskWidgets[deviceNo - 0x31].imagePropertiesLabel->clear();
            diskWidgets[deviceNo - 0x31].ejectAction->setEnabled(false);
            diskWidgets[deviceNo - 0x31].revertAction->setEnabled(false);
            diskWidgets[deviceNo - 0x31].saveAsAction->setEnabled(false);
            diskWidgets[deviceNo - 0x31].editAction->setEnabled(false);
            diskWidgets[deviceNo - 0x31].editAction->setChecked(false);
            diskWidgets[deviceNo - 0x31].autoSaveAction->setEnabled(false);             // Ray A.
            diskWidgets[deviceNo - 0x31].autoSaveAction->setChecked(false);             // Ray A.

        }
    }
}

void MainWindow::uiMessage(int t, QString message)
{
    if (message.at(0) == '"') {
        message.remove(0, 1);
    }
    if (message.at(message.count() - 1) == ' ' && message.at(message.count() - 2) == '"') {
        message.resize(message.count() - 2);
    }

    if (message == lastMessage) {
        lastMessageRepeat++;
        message = QString("%1 [x%2]").arg(message).arg(lastMessageRepeat);
        ui->textEdit->moveCursor(QTextCursor::End);
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
    } else {
        lastMessage = message;
        lastMessageRepeat = 1;
    }

    ui->statusBar->showMessage(message, 3000);

    switch (t) {
        case 'd':
            message = QString("<span style='color:green'>%1</span>").arg(message);
            break;
        case 'u':
            message = QString("<span style='color:gray'>%1</span>").arg(message);
            break;
        case 'n':
            message = QString("<span style='color:black'>%1</span>").arg(message);
            break;
        case 'i':
            message = QString("<span style='color:blue'>%1</span>").arg(message);
            break;
        case 'w':
            message = QString("<span style='color:brown'>%1</span>").arg(message);
            break;
        case 'e':
            message = QString("<span style='color:red'>%1</span>").arg(message);
            break;
        default:
            message = QString("<span style='color:purple'>%1</span>").arg(message);
            break;
    }

    ui->textEdit->append(message);
    ui->textEdit->verticalScrollBar()->setSliderPosition(ui->textEdit->verticalScrollBar()->maximum());
    ui->textEdit->horizontalScrollBar()->setSliderPosition(ui->textEdit->horizontalScrollBar()->minimum());
}

void MainWindow::on_actionOptions_triggered()
{
    bool restart;
    restart = ui->actionStartEmulation->isChecked();
    if (restart) {
        ui->actionStartEmulation->trigger();
        sio->wait();
        qApp->processEvents();
    }
    OptionsDialog optionsDialog(this);
    optionsDialog.exec();

    // load translators and retranslate
    loadTranslators();
    ui->retranslateUi(this);
    for (int i = 0x31; i <= 0x38; i++) {
        deviceStatusChanged(i);
    }
    
    ui->actionStartEmulation->trigger();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog(this, VERSION);
    aboutDialog.exec();
}
// Ray A.
void MainWindow::on_actionDocumentation_triggered()
{
    QString dir = aspeqtSettings->lastSessionDir();

    if (ui->actionDocumentation->isChecked()) {
        docDisplayWindow->show();
    } else {
        docDisplayWindow->hide();
    }
}
// Ray A.
void MainWindow::docDisplayWindowClosed()
{
     ui->actionDocumentation->setChecked(false);
}

void MainWindow::updateRecentFileActions()
{
    ui->actionMountRecent_0->setText(aspeqtSettings->recentImageSetting(0).fileName);
    ui->actionMountRecent_1->setText(aspeqtSettings->recentImageSetting(1).fileName);
    ui->actionMountRecent_2->setText(aspeqtSettings->recentImageSetting(2).fileName);
    ui->actionMountRecent_3->setText(aspeqtSettings->recentImageSetting(3).fileName);
    ui->actionMountRecent_4->setText(aspeqtSettings->recentImageSetting(4).fileName);
    ui->actionMountRecent_5->setText(aspeqtSettings->recentImageSetting(5).fileName);
    ui->actionMountRecent_6->setText(aspeqtSettings->recentImageSetting(6).fileName);
    ui->actionMountRecent_7->setText(aspeqtSettings->recentImageSetting(7).fileName);
    ui->actionMountRecent_8->setText(aspeqtSettings->recentImageSetting(8).fileName);
    ui->actionMountRecent_9->setText(aspeqtSettings->recentImageSetting(9).fileName);

    ui->actionMountRecent_0->setVisible(!ui->actionMountRecent_0->text().isEmpty());
    ui->actionMountRecent_1->setVisible(!ui->actionMountRecent_1->text().isEmpty());
    ui->actionMountRecent_2->setVisible(!ui->actionMountRecent_2->text().isEmpty());
    ui->actionMountRecent_3->setVisible(!ui->actionMountRecent_3->text().isEmpty());
    ui->actionMountRecent_4->setVisible(!ui->actionMountRecent_4->text().isEmpty());
    ui->actionMountRecent_5->setVisible(!ui->actionMountRecent_5->text().isEmpty());
    ui->actionMountRecent_6->setVisible(!ui->actionMountRecent_6->text().isEmpty());
    ui->actionMountRecent_7->setVisible(!ui->actionMountRecent_7->text().isEmpty());
    ui->actionMountRecent_8->setVisible(!ui->actionMountRecent_8->text().isEmpty());
    ui->actionMountRecent_9->setVisible(!ui->actionMountRecent_9->text().isEmpty());
}


bool MainWindow::ejectImage(int no, bool ask)
{
    SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(no + 0x31));

    if (ask && img && img->isModified()) {
        QMessageBox::StandardButton answer;
        answer = saveImageWhenClosing(no, QMessageBox::No, 0);
        if (answer == QMessageBox::Cancel) {
            return false;
        }
    }

    sio->uninstallDevice(no + 0x31);
    if (!img) {
        return true;
    }
    delete img;
    diskWidgets[no].ejectAction->setEnabled(false);
    QString fileName = diskWidgets[no].fileNameLabel->text();
    diskWidgets[no].fileNameLabel->clear();
    diskWidgets[no].writeProtectAction->setChecked(false);
    diskWidgets[no].writeProtectAction->setEnabled(false);
    diskWidgets[no].editAction->setEnabled(false);

    aspeqtSettings->unmountImage(no);
    updateRecentFileActions();
    deviceStatusChanged(no + 0x31);
    qDebug() << "!n" << tr("Unmounted disk %1").arg(no + 1);
    return true;
}

int MainWindow::containingDiskSlot(const QPoint &point)
{
    int i;
    QPoint distance = centralWidget()->geometry().topLeft();
    for (i=0; i < 8; i++) {
        QRect rect = diskWidgets[i].frame->geometry().translated(distance);
        if (rect.contains(point)) {
            break;
        }
    }
    if (i > 7) {
        i = -1;
    }
    return i;
}

int MainWindow::firstEmptyDiskSlot(int startFrom, bool createOne)
{
    int i;
    for (i = startFrom; i < 8; i++) {
        if (!sio->getDevice(0x31 + i)) {
            break;
        }
    }
    if (i > 7) {
        if (createOne) {
            i = 7;
        } else {
            i = -1;
        }
    }
    return i;
}

void MainWindow::bootExe(const QString &fileName)
{
    SioDevice *old = sio->getDevice(0x31);
    AutoBoot loader(sio, old);
    AutoBootDialog dlg(this);

    if (!loader.open(fileName, aspeqtSettings->useHighSpeedExeLoader())) {
        return;
    }

    sio->uninstallDevice(0x31);
    sio->installDevice(0x31, &loader);
    connect(&loader, SIGNAL(booterStarted()), &dlg, SLOT(booterStarted()));
    connect(&loader, SIGNAL(booterLoaded()), &dlg, SLOT(booterLoaded()));
    connect(&loader, SIGNAL(blockRead(int, int)), &dlg, SLOT(blockRead(int, int)));
    connect(&loader, SIGNAL(loaderDone()), &dlg, SLOT(loaderDone()));
    dlg.exec();
    sio->uninstallDevice(0x31);
    if (old) {
        sio->installDevice(0x31, old);
        SimpleDiskImage *d = qobject_cast <SimpleDiskImage*> (old);
        d = qobject_cast <SimpleDiskImage*> (sio->getDevice(0x31));
    }
}

void MainWindow::mountFileWithDefaultProtection(int no, const QString &fileName)
{
    bool prot = aspeqtSettings->getImageSettingsFromName(fileName).isWriteProtected;
    mountFile(no, fileName, prot);
}

void MainWindow::mountFile(int no, const QString &fileName, bool /*prot*/)
{
    SimpleDiskImage *disk;
    bool isDir = false;

    if (fileName.isEmpty()) {
        return;
    }

    FileTypes::FileType type = FileTypes::getFileType(fileName);

    if (type == FileTypes::Dir) {
        disk = new FolderImage(sio);
        isDir = true;
    } else if (type == FileTypes::Pro || type == FileTypes::ProGz) {
        disk = new DiskImagePro(sio);
    } else if (type == FileTypes::Atx || type == FileTypes::AtxGz) {
        disk = new DiskImageAtx(sio);
    } else {
        disk = new SimpleDiskImage(sio);
    }

    if (disk) {
        if (!disk->open(fileName, type)) {
            aspeqtSettings->unmountImage(no);
            delete disk;
            return;
        }
        if (!ejectImage(no)) {
            aspeqtSettings->unmountImage(no);
            delete disk;
            return;
        }

        sio->installDevice(0x31 + no, disk);
        diskWidgets[no].ejectAction->setEnabled(true);
        diskWidgets[no].editAction->setEnabled(true);
        diskWidgets[no].writeProtectAction->setChecked(disk->isReadOnly());
        diskWidgets[no].writeProtectAction->setEnabled(!disk->isUnmodifiable());

        diskWidgets[no].fileNameLabel->setText(fileName);

        aspeqtSettings->mountImage(no, fileName, disk->isReadOnly());
        updateRecentFileActions();
        connect(disk, SIGNAL(statusChanged(int)), this, SLOT(deviceStatusChanged(int)), Qt::QueuedConnection);
        deviceStatusChanged(0x31 + no);

        // Extract the file name without the path // Ray A.
        QString filenamelabel;
        int i = fileName.lastIndexOf("/");
        if (i != -1) {
            i++;
            filenamelabel = fileName.right(fileName.size() - i);
        }

        qDebug() << "!n" << tr("[%1] Mounted '%2' as '%3'.")
                .arg(disk->deviceName())
                .arg(filenamelabel)
//                .arg(fileName)
                .arg(disk->description());
    }
}

void MainWindow::mountDiskImage(int no)
{
    QString dir;

    if (diskWidgets[no].fileNameLabel->text().isEmpty()) {
        dir = aspeqtSettings->lastDiskImageDir();
    } else {
        dir = QFileInfo(diskWidgets[no].fileNameLabel->text()).absolutePath();        
    }
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open a disk image"),
                                                    dir,
                                                    tr(
                                                    "All Atari disk images (*.atr *.xfd *.atx *.pro);;"
                                                    "SIO2PC ATR images (*.atr);;"
                                                    "XFormer XFD images (*.xfd);;"
						    "ATX images (*.atx);;"
                                                    "Pro images (*.pro);;"
                                                    "All files (*)"));
    if (fileName.isEmpty()) {
        return;
    }
    aspeqtSettings->setLastDiskImageDir(QFileInfo(fileName).absolutePath());
    mountFileWithDefaultProtection(no, fileName);
}

void MainWindow::mountFolderImage(int no)
{
    QString dir;
    if (diskWidgets[no].fileNameLabel->text().isEmpty()) {
        dir = aspeqtSettings->lastFolderImageDir();
    } else {
        dir = diskWidgets[no].fileNameLabel->text();
    }
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Open a folder image"), dir);
    if (fileName.isEmpty()) {
        return;
    }
    aspeqtSettings->setLastFolderImageDir(fileName);
    mountFileWithDefaultProtection(no, fileName);
}

void MainWindow::toggleWriteProtection(int no)
{
    SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(no + 0x31));
    if (diskWidgets[no].writeProtectAction->isChecked()) {
        img->setReadOnly(true);
    } else {
        img->setReadOnly(false);
    }
    aspeqtSettings->setMountedImageSetting(no, diskWidgets[no].fileNameLabel->text(), diskWidgets[no].writeProtectAction->isChecked());
}

void MainWindow::openEditor(int no)
{
    SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(no + 0x31));
    if (img->editDialog()) {
        img->editDialog()->close();
    } else {
        DiskEditDialog *dlg = new DiskEditDialog();
        dlg->go(img);
        dlg->show();
    }
}

QMessageBox::StandardButton MainWindow::saveImageWhenClosing(int no, QMessageBox::StandardButton previousAnswer, int number)
{
    SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(no + 0x31));

    if (previousAnswer != QMessageBox::YesToAll) {
        QMessageBox::StandardButtons buttons;
        if (number) {
            buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll | QMessageBox::Cancel;
        } else {
            buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
        }
        previousAnswer = QMessageBox::question(this, tr("Image file unsaved"), tr("'%1' has unsaved changes, do you want to save it?")
                                       .arg(img->originalFileName()), buttons);
    }
    if (previousAnswer == QMessageBox::Yes || previousAnswer == QMessageBox::YesToAll) {
        saveDisk(no);
    }
    if (previousAnswer == QMessageBox::Close) {
        previousAnswer = QMessageBox::Cancel;
    }
    return previousAnswer;
}

void MainWindow::loadTranslators()
{
    qApp->removeTranslator(&aspeqt_qt_translator);
    qApp->removeTranslator(&aspeqt_translator);
    if (aspeqtSettings->i18nLanguage().compare("auto") == 0) {
        QString locale = QLocale::system().name();
        aspeqt_translator.load(":/translations/i18n/aspeqt_" + locale);
        aspeqt_qt_translator.load(":/translations/i18n/qt_" + locale);
        qApp->installTranslator(&aspeqt_qt_translator);
        qApp->installTranslator(&aspeqt_translator);
    } else if (aspeqtSettings->i18nLanguage().compare("en") != 0) {
        aspeqt_translator.load(":/translations/i18n/aspeqt_" + aspeqtSettings->i18nLanguage());
        aspeqt_qt_translator.load(":/translations/i18n/qt_" + aspeqtSettings->i18nLanguage());
        qApp->installTranslator(&aspeqt_qt_translator);
        qApp->installTranslator(&aspeqt_translator);
    }
}

void MainWindow::saveDisk(int no)
{
    SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(no + 0x31));

    if (img->isUnnamed()) {
        saveDiskAs(no);
        return;
    }

    bool saved;

    img->lock();
    saved = img-> save();
    img->unlock();
    if (!saved) {
        if (QMessageBox::question(this, tr("Save failed"), tr("'%1' cannot be saved, do you want to save the image with another name?")
            .arg(img->originalFileName()), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            saveDiskAs(no);
        }
    }
}
// Ray A.
void MainWindow::autoSaveDisk(int no)
{
    SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(no + 0x31));

    if (img->isUnnamed()) {
        saveDiskAs(no);
        diskWidgets[no].saveAction->setEnabled(false);
        diskWidgets[no].revertAction->setEnabled(false);
        return;
    }
    switch (no) {
        case 0 :
            if (ui->autoSave_1->isChecked()) {
                qDebug() << "!d" << tr("[Disk 1] Auto-commit ON.");
            } else {
                qDebug() << "!n" << tr("[Disk 1] Auto-commit OFF.");
              }
            break;

        case 1 :
            if (ui->autoSave_2->isChecked()) {
                qDebug() << "!d" << tr("[Disk 2] Auto-commit ON.");
            } else {
                qDebug() << "!n" << tr("[Disk 2] Auto-commit OFF.");
              }
            break;

        case 2 :
            if (ui->autoSave_3->isChecked()) {
                qDebug() << "!d" << tr("[Disk 3] Auto-commit ON.");
            } else {
                qDebug() << "!n" << tr("[Disk 3] Auto-commit OFF.");
              }
            break;

        case 3 :
            if (ui->autoSave_4->isChecked()) {
                qDebug() << "!d" << tr("[Disk 4] Auto-commit ON.");
            } else {
                qDebug() << "!n" << tr("[Disk 4] Auto-commit OFF.");
              }
            break;

        case 4 :
            if (ui->autoSave_5->isChecked()) {
                qDebug() << "!d" << tr("[Disk 5] Auto-commit ON.");
            } else {
                qDebug() << "!n" << tr("[Disk 5] Auto-commit OFF.");
              }
            break;

        case 5 :
            if (ui->autoSave_6->isChecked()) {
                qDebug() << "!d" << tr("[Disk 6] Auto-commit ON.");
            } else {
                qDebug() << "!n" << tr("[Disk 6] Auto-commit OFF.");
              }
            break;

        case 6 :
            if (ui->autoSave_7->isChecked()) {
                qDebug() << "!d" << tr("[Disk 7] Auto-commit ON.");
            } else {
                qDebug() << "!n" << tr("[Disk 7] Auto-commit OFF.");
              }
            break;

        case 7 :
            if (ui->autoSave_8->isChecked()) {
                qDebug() << "!d" << tr("[Disk 8] Auto-commit ON.");
            } else {
                qDebug() << "!n" << tr("[Disk 8] Auto-commit OFF.");
              }
            break;
    }

    bool saved;

    img->lock();
    saved = img->save();
    img->unlock();
    if (!saved) {
        if (QMessageBox::question(this, tr("Save failed"), tr("'%1' cannot be saved, do you want to save the image with another name?")
            .arg(img->originalFileName()), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            saveDiskAs(no);
        }
    } else {
            diskWidgets[no].saveAction->setEnabled(false);
            diskWidgets[no].revertAction->setEnabled(false);

    }
}
//
void MainWindow::saveDiskAs(int no)
{
    SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(no + 0x31));
    QString dir, fileName;
    bool saved = false;

    if (img->isUnnamed()) {
        dir = aspeqtSettings->lastDiskImageDir();
    } else {
        dir = QFileInfo(img->originalFileName()).absolutePath();
    }

    do {
        fileName = QFileDialog::getSaveFileName(this, tr("Save image as"),
                                 dir,
                                 tr(
                                                    "All Atari disk images (*.atr *.xfd *.atx *.pro);;"
                                                    "SIO2PC ATR images (*.atr);;"
                                                    "XFormer XFD images (*.xfd);;"
						    "ATX images (*.atx);;"
                                                    "Pro images (*.pro);;"
                                                    "All files (*)"));
        if (fileName.isEmpty()) {
            return;
        }

        img->lock();
        saved = img->saveAs(fileName);
        img->unlock();

        if (!saved) {
            if (QMessageBox::question(this, tr("Save failed"), tr("'%1' cannot be saved, do you want to save the image with another name?")
                .arg(fileName), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) {
                break;
            }
        }

    } while (!saved);

    if (saved) {
        aspeqtSettings->setLastDiskImageDir(QFileInfo(fileName).absolutePath());
    }
    aspeqtSettings->unmountImage(no);
    aspeqtSettings->mountImage(no, fileName, img->isReadOnly());
}

void MainWindow::revertDisk(int no)
{
    SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(no + 0x31));
    if (QMessageBox::question(this, tr("Revert to last saved"),
            tr("Do you really want to revert '%1' to its last saved state? You will lose the changes that has been made.")
            .arg(img->originalFileName()), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
        img->lock();
        img->reopen();
        img->unlock();
        deviceStatusChanged(0x31 + no);
    }
}

void MainWindow::on_actionMountDisk_1_triggered() {mountDiskImage(0);}
void MainWindow::on_actionMountDisk_2_triggered() {mountDiskImage(1);}
void MainWindow::on_actionMountDisk_3_triggered() {mountDiskImage(2);}
void MainWindow::on_actionMountDisk_4_triggered() {mountDiskImage(3);}
void MainWindow::on_actionMountDisk_5_triggered() {mountDiskImage(4);}
void MainWindow::on_actionMountDisk_6_triggered() {mountDiskImage(5);}
void MainWindow::on_actionMountDisk_7_triggered() {mountDiskImage(6);}
void MainWindow::on_actionMountDisk_8_triggered() {mountDiskImage(7);}

void MainWindow::on_actionMountFolder_1_triggered() {mountFolderImage(0);}
void MainWindow::on_actionMountFolder_2_triggered() {mountFolderImage(1);}
void MainWindow::on_actionMountFolder_3_triggered() {mountFolderImage(2);}
void MainWindow::on_actionMountFolder_4_triggered() {mountFolderImage(3);}
void MainWindow::on_actionMountFolder_5_triggered() {mountFolderImage(4);}
void MainWindow::on_actionMountFolder_6_triggered() {mountFolderImage(5);}
void MainWindow::on_actionMountFolder_7_triggered() {mountFolderImage(6);}
void MainWindow::on_actionMountFolder_8_triggered() {mountFolderImage(7);}

void MainWindow::on_actionEject_1_triggered() {ejectImage(0);}
void MainWindow::on_actionEject_2_triggered() {ejectImage(1);}
void MainWindow::on_actionEject_3_triggered() {ejectImage(2);}
void MainWindow::on_actionEject_4_triggered() {ejectImage(3);}
void MainWindow::on_actionEject_5_triggered() {ejectImage(4);}
void MainWindow::on_actionEject_6_triggered() {ejectImage(5);}
void MainWindow::on_actionEject_7_triggered() {ejectImage(6);}
void MainWindow::on_actionEject_8_triggered() {ejectImage(7);}

void MainWindow::on_actionWriteProtect_1_triggered() {toggleWriteProtection(0);}
void MainWindow::on_actionWriteProtect_2_triggered() {toggleWriteProtection(1);}
void MainWindow::on_actionWriteProtect_3_triggered() {toggleWriteProtection(2);}
void MainWindow::on_actionWriteProtect_4_triggered() {toggleWriteProtection(3);}
void MainWindow::on_actionWriteProtect_5_triggered() {toggleWriteProtection(4);}
void MainWindow::on_actionWriteProtect_6_triggered() {toggleWriteProtection(5);}
void MainWindow::on_actionWriteProtect_7_triggered() {toggleWriteProtection(6);}
void MainWindow::on_actionWriteProtect_8_triggered() {toggleWriteProtection(7);}

void MainWindow::on_actionMountRecent_0_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_0->text());}
void MainWindow::on_actionMountRecent_1_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_1->text());}
void MainWindow::on_actionMountRecent_2_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_2->text());}
void MainWindow::on_actionMountRecent_3_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_3->text());}
void MainWindow::on_actionMountRecent_4_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_4->text());}
void MainWindow::on_actionMountRecent_5_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_5->text());}
void MainWindow::on_actionMountRecent_6_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_6->text());}
void MainWindow::on_actionMountRecent_7_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_7->text());}
void MainWindow::on_actionMountRecent_8_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_8->text());}
void MainWindow::on_actionMountRecent_9_triggered() {mountFileWithDefaultProtection(firstEmptyDiskSlot(), ui->actionMountRecent_9->text());}

void MainWindow::on_actionEditDisk_1_triggered() {openEditor(0);}
void MainWindow::on_actionEditDisk_2_triggered() {openEditor(1);}
void MainWindow::on_actionEditDisk_3_triggered() {openEditor(2);}
void MainWindow::on_actionEditDisk_4_triggered() {openEditor(3);}
void MainWindow::on_actionEditDisk_5_triggered() {openEditor(4);}
void MainWindow::on_actionEditDisk_6_triggered() {openEditor(5);}
void MainWindow::on_actionEditDisk_7_triggered() {openEditor(6);}
void MainWindow::on_actionEditDisk_8_triggered() {openEditor(7);}

void MainWindow::on_actionSave_1_triggered() {saveDisk(0);}
void MainWindow::on_actionSave_2_triggered() {saveDisk(1);}
void MainWindow::on_actionSave_3_triggered() {saveDisk(2);}
void MainWindow::on_actionSave_4_triggered() {saveDisk(3);}
void MainWindow::on_actionSave_5_triggered() {saveDisk(4);}
void MainWindow::on_actionSave_6_triggered() {saveDisk(5);}
void MainWindow::on_actionSave_7_triggered() {saveDisk(6);}
void MainWindow::on_actionSave_8_triggered() {saveDisk(7);}
// Ray A.
void MainWindow::on_actionAutoSave_1_triggered() {autoSaveDisk(0);}
void MainWindow::on_actionAutoSave_2_triggered() {autoSaveDisk(1);}
void MainWindow::on_actionAutoSave_3_triggered() {autoSaveDisk(2);}
void MainWindow::on_actionAutoSave_4_triggered() {autoSaveDisk(3);}
void MainWindow::on_actionAutoSave_5_triggered() {autoSaveDisk(4);}
void MainWindow::on_actionAutoSave_6_triggered() {autoSaveDisk(5);}
void MainWindow::on_actionAutoSave_7_triggered() {autoSaveDisk(6);}
void MainWindow::on_actionAutoSave_8_triggered() {autoSaveDisk(7);}
//
void MainWindow::on_actionSaveAs_1_triggered() {saveDiskAs(0);}
void MainWindow::on_actionSaveAs_2_triggered() {saveDiskAs(1);}
void MainWindow::on_actionSaveAs_3_triggered() {saveDiskAs(2);}
void MainWindow::on_actionSaveAs_4_triggered() {saveDiskAs(3);}
void MainWindow::on_actionSaveAs_5_triggered() {saveDiskAs(4);}
void MainWindow::on_actionSaveAs_6_triggered() {saveDiskAs(5);}
void MainWindow::on_actionSaveAs_7_triggered() {saveDiskAs(6);}
void MainWindow::on_actionSaveAs_8_triggered() {saveDiskAs(7);}

void MainWindow::on_actionRevert_1_triggered() {revertDisk(0);}
void MainWindow::on_actionRevert_2_triggered() {revertDisk(1);}
void MainWindow::on_actionRevert_3_triggered() {revertDisk(2);}
void MainWindow::on_actionRevert_4_triggered() {revertDisk(3);}
void MainWindow::on_actionRevert_5_triggered() {revertDisk(4);}
void MainWindow::on_actionRevert_6_triggered() {revertDisk(5);}
void MainWindow::on_actionRevert_7_triggered() {revertDisk(6);}
void MainWindow::on_actionRevert_8_triggered() {revertDisk(7);}

void MainWindow::on_actionEjectAll_triggered()
{
    QMessageBox::StandardButton answer = QMessageBox::No;

    int toBeSaved = 0;

    for (int i = 0; i < 8; i++) {
        SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(i + 0x31));
        if (img && img->isModified()) {
            toBeSaved++;
        }
    }

    if (!toBeSaved) {
        for (int i = 7; i >= 0; i--) {
            ejectImage(i);
        }
        return;
    }

    bool wasRunning = ui->actionStartEmulation->isChecked();
    if (wasRunning) {
        ui->actionStartEmulation->trigger();
    }

    for (int i = 7; i >= 0; i--) {
        SimpleDiskImage *img = qobject_cast <SimpleDiskImage*> (sio->getDevice(i + 0x31));
        if (img && img->isModified()) {
            toBeSaved--;
            answer = saveImageWhenClosing(i, answer, toBeSaved);
            if (answer == QMessageBox::NoToAll) {
                break;
            }
            if (answer == QMessageBox::Cancel) {
                if (wasRunning) {
                    ui->actionStartEmulation->trigger();
                }
                return;
            }
        }
    }
    for (int i = 7; i >= 0; i--) {
        ejectImage(i, false);
    }
    if (wasRunning) {
        ui->actionStartEmulation->trigger();
    }
}
void MainWindow::on_actionMountDisk_triggered()
{
    mountDiskImage(firstEmptyDiskSlot(0, true));
}

void MainWindow::on_actionMountFolder_triggered()
{
    mountFolderImage(firstEmptyDiskSlot(0, true));
}

void MainWindow::on_actionNewImage_triggered()
{
    CreateImageDialog dlg(this);
    if (!dlg.exec()) {
        return;
    };

    SimpleDiskImage *disk = new SimpleDiskImage(sio);
    connect(disk, SIGNAL(statusChanged(int)), this, SLOT(deviceStatusChanged(int)), Qt::QueuedConnection);

    if (!disk->create(++untitledName)) {
        delete disk;
        return;
    }

    DiskGeometry g;
    uint size = dlg.sectorCount() * dlg.sectorSize();
    if (dlg.sectorSize() == 256) {
        if (dlg.sectorCount() >= 3) {
            size -= 384;
        } else {
            size -= dlg.sectorCount() * 128;
        }
    }
    g.initialize(size, dlg.sectorSize());

    if (!disk->format(g)) {
        delete disk;
        return;
    }

    int no = firstEmptyDiskSlot(0, true);

    if (!ejectImage(no)) {
        delete disk;
        return;
    }

    sio->installDevice(0x31 + no, disk);
    deviceStatusChanged(0x31 + no);
    qDebug() << "!n" << tr("[%1] Mounted '%2' as '%3'.")
            .arg(disk->deviceName())
            .arg(disk->originalFileName())
            .arg(disk->description());
}

void MainWindow::on_actionOpenSession_triggered()
{
    QString dir = aspeqtSettings->lastSessionDir();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open session"),
                                 dir,
                                 tr(
                                         "AspeQt sessions (*.aspeqt);;"
                                         "All files (*)"));
    if (fileName.isEmpty()) {
        return;
    }
    aspeqtSettings->setLastSessionDir(QFileInfo(fileName).absolutePath());
    aspeqtSettings->loadSessionFromFile(fileName);
    for (int i = 0; i < 8; i++) {
        AspeqtSettings::ImageSettings is;
        is = aspeqtSettings->mountedImageSetting(i);
        mountFile(i, is.fileName, is.isWriteProtected);
    }
}

void MainWindow::on_actionSaveSession_triggered()
{
    QString dir = aspeqtSettings->lastSessionDir();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save session as"),
                                 dir,
                                 tr(
                                         "AspeQt sessions (*.aspeqt);;"
                                         "All files (*)"));
    if (fileName.isEmpty()) {
        return;
    }
    aspeqtSettings->setLastSessionDir(QFileInfo(fileName).absolutePath());
    aspeqtSettings->saveSessionToFile(fileName);
}

void MainWindow::on_actionBootExe_triggered()
{
    QString dir = aspeqtSettings->lastExeDir();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open executable"),
                                 dir,
                                 tr(
                                         "Atari executables (*.xex *.com *.exe);;"
                                         "All files (*)"));
    if (fileName.isEmpty()) {
        return;
    }
    aspeqtSettings->setLastExeDir(QFileInfo(fileName).absolutePath());
    bootExe(fileName);
}

void MainWindow::on_actionShowPrinterTextOutput_triggered()
{
    if (ui->actionShowPrinterTextOutput->isChecked()) {
        textPrinterWindow->show();
    } else {
        textPrinterWindow->hide();
    }
}

void MainWindow::textPrinterWindowClosed()
{
    ui->actionShowPrinterTextOutput->setChecked(false);
}

void MainWindow::on_actionPlaybackCassette_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open a cassette image"),
                                                    aspeqtSettings->lastCasDir(),
                                                    tr(
                                                    "CAS images (*.cas);;"
                                                    "All files (*)"));

    if (fileName.isEmpty()) {
        return;
    }
    aspeqtSettings->setLastCasDir(QFileInfo(fileName).absolutePath());

    bool restart;
    restart = ui->actionStartEmulation->isChecked();
    if (restart) {
        ui->actionStartEmulation->trigger();
        sio->wait();
        qApp->processEvents();
    }

    CassetteDialog *dlg = new CassetteDialog(this, fileName);
    dlg->exec();
    delete dlg;

    if (restart) {
        ui->actionStartEmulation->trigger();
    }
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        setWindowFlags(oldWindowFlags);
        setWindowState(oldWindowStates);
        show();
        activateWindow();
        raise();
        trayIcon.hide();
    }
}
