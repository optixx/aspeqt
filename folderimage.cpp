#include "folderimage.h"

#include <QFileInfoList>
#include <QtDebug>

FolderImage::~FolderImage()
{
    close();
}

void FolderImage::close()
{
    for (int i = 0; i < 64; i++) {
        atariFiles[i].exists = false;
    }

    return;
}

bool FolderImage::format(quint16, quint16)
{
    return false;
}

void FolderImage::buildDirectory()
{
    QFileInfoList infos = dir.entryInfoList(QDir::Files,  QDir::Name);
    QFileInfo info;
    QString name;
    QString ext;

    int j = -1, k, i;
    for (i = 0; i < 64; i++) {
        do {
            j++;
            if (j >= infos.count()) {
                atariFiles[i].exists = false;
                break;
            }
            info = infos.at(j);
            name = info.completeBaseName().toUpper();
            name.remove(QRegExp("[^A-Z0-9]"));
            name = name.left(8);
            if (name.isEmpty()) {
                name = "BADNAME";
            }
            ext = info.suffix().toUpper();
            ext.remove(QRegExp("[^A-Z0-9]"));
            ext = ext.left(3);
            QString baseName = name.left(7);

            int l = 2;
            do {
                for (k = 0; k < i; k++) {
                    if (atariFiles[k].atariName == name && atariFiles[k].atariExt == ext) {
                        break;
                    }
                }
                if (k < i) {
                    name = QString("%1%2").arg(baseName).arg(l);
                    l++;
                }
            } while (k < i && l < 10000000);
            if (l == 10) {baseName = name.left(6);}
            if (l == 100) {baseName = name.left(5);}
            if (l == 1000) {baseName = name.left(4);}
            if (l == 10000) {baseName = name.left(3);}
            if (l == 100000) {baseName = name.left(2);}
            if (l == 1000000) {baseName = name.left(1);}
            if (l == 10000000) {baseName = "";}
            if (l == 100000000) {
                qWarning() << "!w" << tr("Cannot mirror '%1' in '%2': No suitable Atari name can be found.")
                               .arg(info.fileName())
                               .arg(dir.path());
            }
        } while (k < i);

        if (j >= infos.count()) {
            break;
        }

        atariFiles[i].exists = true;
        atariFiles[i].original = info;
        atariFiles[i].atariName = name;
        atariFiles[i].atariExt = ext;
        atariFiles[i].lastSector = 0;
    }

    if (i < infos.count()) {
        qWarning() << "!w" << tr("Cannot mirror %1 of %2 files in '%3': Atari directory is full.")
                       .arg(infos.count() - i)
                       .arg(infos.count())
                       .arg(dir.path());
    }
}

bool FolderImage::open(const QString &fileName, FileTypes::FileType /* type */)
{
    dir.setPath(fileName);

    buildDirectory();

    m_originalFileName = fileName;
    m_geometry.initialize(false, 40, 18, 128);
    m_newGeometry.initialize(m_geometry);
    m_isReadOnly = true;
    m_isModified = false;
    m_isUnmodifiable = true;
    return true;
}

bool FolderImage::readSector(quint16 sector, QByteArray &data)
{
    /* Boot */
    if (sector == 1) {
        data = QByteArray(128, 0);
        data[1] = 0x03;
        data[3] = 0x07;
        data[4] = 0x40;
        data[5] = 0x15;
        data[6] = 0x4c; data[7] = 0x14; data[8] = 0x07; // JMP 0x0714

        data[0x14] = 0x38;  // SEC
        data[0x15] = 0x60;  // RTS

        return true;
    }

    /* VTOC */
    if (sector == 360) {
        data = QByteArray(128, 0);
        data[0] = 2;
        data[1] = 707 % 256;
        data[2] = 707 / 256;
        for (int i = 10; i < 100; i++) {
            data[i] = 0xff;
        }
        return true;
    }

    /* Directory sectors */
    if (sector >= 361 && sector <=368) {
        if (sector == 361) {
            buildDirectory();
        }
        data.resize(0);
        for (int i = (sector - 361) * 8; i < (sector - 360) * 8; i++) {
            QByteArray entry;
            if (!atariFiles[i].exists) {
                entry = QByteArray(16, 0);
            } else {
                entry = "";
                entry[0] = 0x42;
                QFileInfo info = atariFiles[i].original;;
                int size = (info.size() + 124) / 125;
                if (size > 999) {
                    size = 999;
                }
                entry[1] = size % 256;
                entry[2] = size / 256;
                int first = 400 + i*5;
                entry[3] = first % 256;
                entry[4] = first / 256;
                entry += atariFiles[i].atariName.toAscii();
                while (entry.count() < 13) {
                    entry += 32;
                }
                entry += atariFiles[i].atariExt.toAscii();
                while (entry.count() < 16) {
                    entry += 32;
                }
            }
            data += entry;
        }
        return true;
    }

    /* Data sectors */
    if (sector >= 400 && sector < 720) {
        int fileNo = (sector - 400) / 5;
        if (!atariFiles[fileNo].exists) {
            data = QByteArray(128, 0);
            return true;
        }
        QFile file(atariFiles[fileNo].original.absoluteFilePath());
        file.open(QFile::ReadOnly);

        /* First sector */
        if (fileNo * 5 + 400 == sector) {
            atariFiles[fileNo].pos = 0;
        }
        file.seek(atariFiles[fileNo].pos);
        data = file.read(125);
        int next = sector + 1;

        /* Last sector */
        if (fileNo * 5 + 404 == sector) {
            next = sector - 3;
        }
        int size = data.size();
        data.resize(128);
        if (atariFiles[fileNo].lastSector != sector) {
            atariFiles[fileNo].pos += size;
        }
        atariFiles[fileNo].lastSector = sector;
        if (file.atEnd()) {
            next = 0;
        }
        data[125] = (fileNo * 4) | (next / 256);
        data[126] = next % 256;
        data[127] = size;
        return true;
    }

    /* Any other sector */

    data = QByteArray(128, 0);
    return true;
}

bool FolderImage::writeSector(quint16, const QByteArray &)
{
    return false;
}
