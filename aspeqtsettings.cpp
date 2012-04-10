#include "aspeqtsettings.h"
#include "serialport.h"

AspeqtSettings::AspeqtSettings()
{
    mSettings = new QSettings();

    mIsFirstTime = mSettings->value("FirstTime", true).toBool();
    mSettings->setValue("FirstTime", false);

    // Set initial Main Window Positions and Size// Ray A.
    mXpos = mSettings->value("MainX").toInt();
    if (mXpos == 0) {
        mSettings->setValue("MainX", 10);
        mXpos = 10;
    }
    mYpos = mSettings->value("MainY").toInt();
    if (mYpos == 0) {
        mSettings->setValue("MainY", 10);
        mYpos = 10;
    }
    mWidth = mSettings->value("MainW").toInt();
    if (mWidth == 0) {
        mSettings->setValue("MainW", 600);
        mWidth = 600;
    }
    mHeight = mSettings->value("MainH").toInt();
    if (mHeight == 0) {
        mSettings->setValue("MainH", 486);
        mHeight = 486;
    }

    /* Standard serial port backend */
    mSerialPortName = mSettings->value("SerialPortName", StandardSerialPortBackend::defaultPortName()).toString();
    mSerialPortHandshakingMethod = mSettings->value("HandshakingMethod", 0).toInt();
    mSerialPortMaximumSpeed = mSettings->value("MaximumSerialPortSpeed", 2).toInt();
    mSerialPortUsePokeyDivisors = mSettings->value("SerialPortUsePokeyDivisors", false).toBool();
    mSerialPortPokeyDivisor = mSettings->value("SerialPortPokeyDivisor", 6).toInt();

    mUseHighSpeedExeLoader = mSettings->value("UseHighSpeedExeLoader", false).toBool();

    mAtariSioDriverName = mSettings->value("AtariSioDriverName", AtariSioBackend::defaultPortName()).toString();
    mAtariSioHandshakingMethod = mSettings->value("AtariSioHandshakingMethod", 0).toInt();

    mBackend = mSettings->value("Backend", 0).toInt();

    mUseCustomCasBaud = mSettings->value("UseCustomCasBaud", false).toBool();
    mCustomCasBaud = mSettings->value("CustomCasBaud", 875).toInt();

    int i;

    mSettings->beginReadArray("MountedImageSettings");
    for (i = 0; i < 8; i++) {
        mSettings->setArrayIndex(i);
        mMountedImageSettings[i].fileName = mSettings->value("FileName", QString()).toString();
        mMountedImageSettings[i].isWriteProtected = mSettings->value("IsWriteProtected", false).toBool();
    }
    mSettings->endArray();

    mSettings->beginReadArray("RecentImageSettings");
    for (i = 0; i < 10; i++) {
        mSettings->setArrayIndex(i);
        mRecentImageSettings[i].fileName = mSettings->value("FileName", QString()).toString();
        mRecentImageSettings[i].isWriteProtected = mSettings->value("IsWriteProtected", false).toBool();
    }
    mSettings->endArray();

    mLastDiskImageDir = mSettings->value("LastDiskImageDir", "").toString();
    mLastFolderImageDir = mSettings->value("LastFolderImageDir", "").toString();
    mLastSessionDir = mSettings->value("LastSessionDir", "").toString();
    mLastExeDir = mSettings->value("LastExeDir", "").toString();
    mLastExtractDir = mSettings->value("LastExtractDir", "").toString();
    mLastPrinterTextDir = mSettings->value("LastPrinterTextDir", "").toString();
    mLastCasDir = mSettings->value("LastCasDir", "").toString();
    
    mI18nLanguage = mSettings->value("I18nLanguage", "auto").toString();

    mMinimizeToTray = mSettings->value("MinimizeToTray", false).toBool();
}

AspeqtSettings::~AspeqtSettings()
{
    delete mSettings;
}

void AspeqtSettings::saveSessionToFile(const QString &fileName)
{
    QSettings s(fileName, QSettings::IniFormat);
    s.beginWriteArray("MountedImageSettings");
    for (int i = 0; i < 8; i++) {
        ImageSettings is = mMountedImageSettings[i];
        s.setArrayIndex(i);
        s.setValue("FileName", is.fileName);
        s.setValue("IsWriteProtected", is.isWriteProtected);
    }
    s.endArray();
}

void AspeqtSettings::loadSessionFromFile(const QString &fileName)
{
    QSettings s(fileName, QSettings::IniFormat);
    s.beginReadArray("MountedImageSettings");
    for (int i = 0; i < 8; i++) {
        s.setArrayIndex(i);
        setMountedImageSetting(i, s.value("FileName", "").toString(), s.value("IsWriteProtected", false).toBool());
    }
    s.endArray();
}

bool AspeqtSettings::isFirstTime()
{
    return mIsFirstTime;
}

QString AspeqtSettings::serialPortName()
{
    return mSerialPortName;
}

void AspeqtSettings::setSerialPortName(const QString &name)
{
    mSerialPortName = name;
    mSettings->setValue("SerialPortName", mSerialPortName);
}

QString AspeqtSettings::atariSioDriverName()
{
    return mAtariSioDriverName;
}

void AspeqtSettings::setAtariSioDriverName(const QString &name)
{
    mAtariSioDriverName = name;
    mSettings->setValue("AtariSioDriverName", mAtariSioDriverName);
}

int AspeqtSettings::atariSioHandshakingMethod()
{
    return mAtariSioHandshakingMethod;
}

void AspeqtSettings::setAtariSioHandshakingMethod(int method)
{
    mAtariSioHandshakingMethod = method;
    mSettings->setValue("AtariSioHandshakingMethod", mAtariSioHandshakingMethod);
}

int AspeqtSettings::serialPortMaximumSpeed()
{
    return mSerialPortMaximumSpeed;
}

void AspeqtSettings::setSerialPortMaximumSpeed(int speed)
{
    mSerialPortMaximumSpeed = speed;
    mSettings->setValue("MaximumSerialPortSpeed", mSerialPortMaximumSpeed);
}

bool AspeqtSettings::serialPortUsePokeyDivisors()
{
    return mSerialPortUsePokeyDivisors;
}

void AspeqtSettings::setSerialPortUsePokeyDivisors(bool use)
{
    mSerialPortUsePokeyDivisors = use;
    mSettings->setValue("SerialPortUsePokeyDivisors", mSerialPortUsePokeyDivisors);
}

int AspeqtSettings::serialPortPokeyDivisor()
{
    return mSerialPortPokeyDivisor;
}

void AspeqtSettings::setSerialPortPokeyDivisor(int divisor)
{
    mSerialPortPokeyDivisor = divisor;
    mSettings->setValue("SerialPortPokeyDivisor", mSerialPortPokeyDivisor);
}

int AspeqtSettings::serialPortHandshakingMethod()
{
    return mSerialPortHandshakingMethod;
}

void AspeqtSettings::setSerialPortHandshakingMethod(int method)
{
    mSerialPortHandshakingMethod = method;
    mSettings->setValue("HandshakingMethod", mSerialPortHandshakingMethod);
}

int AspeqtSettings::backend()
{
    return mBackend;
}

void AspeqtSettings::setBackend(int backend)
{
    mBackend = backend;
    mSettings->setValue("Backend", mBackend);
}

bool AspeqtSettings::useHighSpeedExeLoader()
{
    return mUseHighSpeedExeLoader;
}

void AspeqtSettings::setUseHighSpeedExeLoader(bool use)
{
    mUseHighSpeedExeLoader = use;
    mSettings->setValue("UseHighSpeedExeLoader", mUseHighSpeedExeLoader);
}

bool AspeqtSettings::useCustomCasBaud()
{
    return mUseCustomCasBaud;
}

void AspeqtSettings::setUseCustomCasBaud(bool use)
{
    mUseCustomCasBaud = use;
    mSettings->setValue("UseCustomCasBaud", mUseCustomCasBaud);
}

int AspeqtSettings::customCasBaud()
{
    return mCustomCasBaud;
}

void AspeqtSettings::setCustomCasBaud(int baud)
{
    mCustomCasBaud = baud;
    mSettings->setValue("CustomCasBaud", mCustomCasBaud);
}

AspeqtSettings::ImageSettings AspeqtSettings::getImageSettingsFromName(const QString &fileName)
{
    ImageSettings is;
    int i;
    bool found = false;

    for (i = 0; i < 8; i++) {
        if (mMountedImageSettings[i].fileName == fileName) {
            is = mMountedImageSettings[i];
            found = true;
            break;
        }
    }
    if (!found) {
        for (i = 0; i < 10; i++) {
            if (mRecentImageSettings[i].fileName == fileName) {
                is = mRecentImageSettings[i];
                found = true;
                break;
            }
        }
    }
    if (!found) {
        is.fileName = fileName;
        is.isWriteProtected = true;
    }
    return is;
}

AspeqtSettings::ImageSettings AspeqtSettings::mountedImageSetting(int no)
{
    return mMountedImageSettings[no];
}

AspeqtSettings::ImageSettings AspeqtSettings::recentImageSetting(int no)
{
    return mRecentImageSettings[no];
}

void AspeqtSettings::setMountedImageSetting(int no, const QString &fileName, bool prot)
{
    mMountedImageSettings[no].fileName = fileName;
    mMountedImageSettings[no].isWriteProtected = prot;
    mSettings->setValue(QString("MountedImageSettings/%1/FileName").arg(no+1), fileName);
    mSettings->setValue(QString("MountedImageSettings/%1/IsWriteProtected").arg(no+1), prot);
}

void AspeqtSettings::mountImage(int no, const QString &fileName, bool prot)
{
    if (fileName.isEmpty()) {
        return;
    }
    int i;
    bool found = false;
    for (i = 0; i < 10; i++) {
        if (mRecentImageSettings[i].fileName == fileName) {
            found = true;
            break;
        }
    }
    if (found) {
        for (int j = i; j < 9; j++) {
            mRecentImageSettings[j] = mRecentImageSettings[j + 1];
        }
        mRecentImageSettings[9].fileName = "";
        writeRecentImageSettings();
    }
    setMountedImageSetting(no, fileName, prot);
}

void AspeqtSettings::unmountImage(int no)
{
    ImageSettings is = mMountedImageSettings[no];

    for (int i = 9; i > 0; i--) {
            mRecentImageSettings[i] = mRecentImageSettings[i - 1];
    }
    mRecentImageSettings[0] = is;
    writeRecentImageSettings();

    setMountedImageSetting(no, "", false);
}

void AspeqtSettings::swapImages(int no1, int no2)
{
    ImageSettings is1 = mountedImageSetting(no1);
    ImageSettings is2 = mountedImageSetting(no2);
    setMountedImageSetting(no1, is2.fileName, is2.isWriteProtected);
    setMountedImageSetting(no2, is1.fileName, is1.isWriteProtected);
}

// Window Position and Size // Ray A.

int AspeqtSettings::lastHorizontalPos()
{
    return mXpos;
}

void AspeqtSettings::setLastHorizontalPos(int lastHpos)
{
    mXpos = lastHpos;
    mSettings->setValue("MainX", mXpos);
}
int AspeqtSettings::lastVerticalPos()
{
    return mYpos;
}

void AspeqtSettings::setLastVerticalPos(int lastVpos)
{
    mYpos = lastVpos;
    mSettings->setValue("MainY", mYpos);
}
int AspeqtSettings::lastWidth()
{
    return mWidth;
}

void AspeqtSettings::setLastWidth(int lastW)
{
    mWidth = lastW;
    mSettings->setValue("MainW", mWidth);
}
int AspeqtSettings::lastHeight()
{
    return mHeight;
}

void AspeqtSettings::setLastHeight(int lastH)
{
    mHeight = lastH;
    mSettings->setValue("MainH", mHeight);
}
QString AspeqtSettings::lastDiskImageDir()
{
    return mLastDiskImageDir;
}

void AspeqtSettings::setLastDiskImageDir(const QString &dir)
{
    mLastDiskImageDir = dir;
    mSettings->setValue("LastDiskImageDir", mLastDiskImageDir);
}

QString AspeqtSettings::lastFolderImageDir()
{
    return mLastFolderImageDir;
}

void AspeqtSettings::setLastFolderImageDir(const QString &dir)
{
    mLastFolderImageDir = dir;
    mSettings->setValue("LastFolderImageDir", mLastFolderImageDir);
}

QString AspeqtSettings::lastSessionDir()
{
    return mLastSessionDir;
}

void AspeqtSettings::setLastSessionDir(const QString &dir)
{
    mLastSessionDir = dir;
    mSettings->setValue("LastSessionDir", mLastFolderImageDir);
}

QString AspeqtSettings::lastExeDir()
{
    return mLastExeDir;
}

void AspeqtSettings::setLastExeDir(const QString &dir)
{
    mLastExeDir = dir;
    mSettings->setValue("LastExeDir", mLastExeDir);
}

QString AspeqtSettings::lastExtractDir()
{
    return mLastExtractDir;
}

void AspeqtSettings::setLastExtractDir(const QString &dir)
{
    mLastExtractDir = dir;
    mSettings->setValue("LastExtractDir", mLastExeDir);
}

QString AspeqtSettings::lastPrinterTextDir()
{
    return mLastPrinterTextDir;
}

void AspeqtSettings::setLastPrinterTextDir(const QString &dir)
{
    mLastPrinterTextDir = dir;
    mSettings->setValue("LastPrinterTextDir", mLastPrinterTextDir);
}

QString AspeqtSettings::lastCasDir()
{
    return mLastCasDir;
}

void AspeqtSettings::setLastCasDir(const QString &dir)
{
    mLastCasDir = dir;
    mSettings->setValue("LastCasDir", mLastCasDir);
}

QString AspeqtSettings::i18nLanguage()
{
    return mI18nLanguage;
}

void AspeqtSettings::setI18nLanguage(const QString &lang)
{
    mI18nLanguage = lang;
    mSettings->setValue("I18nLanguage", mI18nLanguage);
}

bool AspeqtSettings::minimizeToTray()
{
    return mMinimizeToTray;
}

void AspeqtSettings::setMinimizeToTray(bool tray)
{
    mMinimizeToTray = tray;
    mSettings->setValue("MinimizeToTray", mMinimizeToTray);
}

void AspeqtSettings::writeRecentImageSettings()
{
    mSettings->beginWriteArray("RecentImageSettings");
    for (int i = 0; i < 10; i++) {
        mSettings->setArrayIndex(i);
        mSettings->setValue("FileName", mRecentImageSettings[i].fileName);
        mSettings->setValue("IsWriteProtected", mRecentImageSettings[i].isWriteProtected);
    }
    mSettings->endArray();
}
