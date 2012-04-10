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

    void saveSessionToFile(const QString &fileName);
    void loadSessionFromFile(const QString &fileName);

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

    QString i18nLanguage();
    void setI18nLanguage(const QString &$lang);

    bool minimizeToTray();
    void setMinimizeToTray(bool tray);
    
private:
    QSettings *mSettings;

    void writeRecentImageSettings();

    bool mIsFirstTime;

    // Ray A.
    int mXpos;
    int mYpos;
    int mWidth;
    int mHeight;

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

    ImageSettings mMountedImageSettings[8];
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
};

extern AspeqtSettings *aspeqtSettings;

#endif // ASPEQTSETTINGS_H
