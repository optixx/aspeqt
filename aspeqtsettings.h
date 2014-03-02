#ifndef ASPEQTSETTINGS_H
#define ASPEQTSETTINGS_H

#include <QSettings>

class AspeqtSettings
{
public:
    class ImageSettings {
    public:
        QString fileName;
        bool isWriteProtected;
    };

    AspeqtSettings();
    ~AspeqtSettings();

    bool isFirstTime();

    QString serialPortName();
    void setSerialPortName(const QString &name);

    int serialPortHandshakingMethod();
    void setSerialPortHandshakingMethod(int method);

    int serialPortMaximumSpeed();
    void setSerialPortMaximumSpeed(int speed);

    bool serialPortUsePokeyDivisors();
    void setSerialPortUsePokeyDivisors(bool use);

    int serialPortPokeyDivisor();
    void setSerialPortPokeyDivisor(int divisor);

    bool useHighSpeedExeLoader();
    void setUseHighSpeedExeLoader(bool use);

    QString atariSioDriverName();
    void setAtariSioDriverName(const QString &name);

    int atariSioHandshakingMethod();
    void setAtariSioHandshakingMethod(int method);

    int backend();
    void setBackend(int backend);

    bool useCustomCasBaud();
    void setUseCustomCasBaud(bool use);

    int customCasBaud();
    void setCustomCasBaud(int baud);

    ImageSettings getImageSettingsFromName(const QString &fileName);

    ImageSettings mountedImageSetting(int no);

    void setMountedImageSetting(int no, const QString &fileName, bool prot);
    ImageSettings recentImageSetting(int no);

    void mountImage(int no, const QString &fileName, bool prot);

    void unmountImage(int no);

    void swapImages(int no1, int no2);

    QString lastDiskImageDir();
    void setLastDiskImageDir(const QString &dir);

    QString lastFolderImageDir();
    void setLastFolderImageDir(const QString &dir);

    QString lastSessionDir();
    void setLastSessionDir(const QString &dir);

    QString lastExeDir();
    void setLastExeDir(const QString &dir);

    QString lastExtractDir();
    void setLastExtractDir(const QString &dir);

    QString lastPrinterTextDir();
    void setLastPrinterTextDir(const QString &dir);

    QString lastCasDir();
    void setLastCasDir(const QString &dir);

    // Set and restore last mainwindow position and size // Ray A.
    int lastVerticalPos();
    void setLastVerticalPos(int lastVpos);

    int lastHorizontalPos();
    void setLastHorizontalPos(int lastHpos);

    int lastWidth();
    void setLastWidth(int lastW);

    int lastHeight();
    void setLastHeight(int lastH);

    // Set and restore last printwindow position and size // Ray A.
    int lastPrtVerticalPos();
    void setLastPrtVerticalPos(int lastVpos);

    int lastPrtHorizontalPos();
    void setLastPrtHorizontalPos(int lastHpos);

    int lastPrtWidth();
    void setLastPrtWidth(int lastW);

    int lastPrtHeight();
    void setLastPrtHeight(int lastH);

    QString i18nLanguage();
    void setI18nLanguage(const QString &$lang);

    bool minimizeToTray();
    void setMinimizeToTray(bool tray);

// Save window positions and sizes option // Ray A.
    bool saveWindowsPos();
    void setsaveWindowsPos(bool saveMwp);

// To pass session file name/path  // Ray A.
    void setSessionFile(const QString &g_sessionFile, const QString &g_sessionFilePath);

// To manipulate session files  // Ray A.
    void saveSessionToFile(const QString &fileName);
    void loadSessionFromFile(const QString &fileName);

// To manipulate Main Window Title for Session file names    // Ray A.
    void setMainWindowTitle(const QString &g_mainWindowTitle);

// Show more drives & Hide message log   // Ray A.
    bool showMoreDrives ();
    void setShowMoreDrives (bool checked);

    bool hideMessageLog ();
    void setHideMessageLog (bool checked);

// Filter special characters from file names in Folder Images
    bool filterUnderscore();
    void setfilterUnderscore(bool filter);

private:
    QSettings *mSettings;

    void writeRecentImageSettings();

    bool mIsFirstTime;

// To pass values from Mainwindow // Ray A.
    int mMainX;
    int mMainY;
    int mMainW;
    int mMainH;
    int mPrtX;
    int mPrtY;
    int mPrtW;
    int mPrtH;

    bool msaveWindowsPos;
    QString mSessionFileName;
    QString mSessionFilePath;
    QString mMainWindowTitle;
//
    QString mSerialPortName;
    int mSerialPortHandshakingMethod;
    int mSerialPortMaximumSpeed;
    bool mSerialPortUsePokeyDivisors;
    int mSerialPortPokeyDivisor;

    bool mUseHighSpeedExeLoader;

    QString mAtariSioDriverName;
    int mAtariSioHandshakingMethod;

    int mBackend;

    bool mUseCustomCasBaud;
    int mCustomCasBaud;

    ImageSettings mMountedImageSettings[16];    // Ray A.

    ImageSettings mRecentImageSettings[10];
    QString mLastDiskImageDir;
    QString mLastFolderImageDir;
    QString mLastSessionDir;
    QString mLastExeDir;
    QString mLastExtractDir;
    QString mLastPrinterTextDir;
    QString mLastCasDir;
    
    QString mI18nLanguage;

    bool mMinimizeToTray;
    bool mFilterUnderscore;
};

extern AspeqtSettings *aspeqtSettings;

#endif // ASPEQTSETTINGS_H
