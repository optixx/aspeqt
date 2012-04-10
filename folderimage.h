#ifndef FOLDERIMAGE_H
#define FOLDERIMAGE_H

#include <QDir>

#include "diskimage.h"

class AtariFile
{
public:
    bool exists;
    QFileInfo original;
    QString atariName;
    QString atariExt;
    quint64 pos;
    int lastSector;
};

class FolderImage : public SimpleDiskImage
{
    Q_OBJECT

protected:
    QDir dir;
    bool mReadOnly;
    AtariFile atariFiles[64];
    void buildDirectory();

public:
    FolderImage(SioWorker *worker): SimpleDiskImage(worker) {}
    ~FolderImage();

    void close();
    bool open(const QString &fileName, FileTypes::FileType /* type */);
    bool readSector(quint16 sector, QByteArray &data);
    bool writeSector(quint16 sector, const QByteArray &data);
    bool format(quint16 aSectorCount, quint16 aSectorSize);
    QString description() const {return tr("Folder image");}
};

#endif // FOLDERIMAGE_H
