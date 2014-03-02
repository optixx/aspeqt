#ifndef MISCDEVICES_H
#define MISCDEVICES_H

#include "sioworker.h"

class Printer: public SioDevice
{
    Q_OBJECT
private:
    int m_lastOperation;
public:
    Printer(SioWorker *worker): SioDevice(worker) {}
    void handleCommand(quint8 command, quint16 aux);
signals:
    void print(const QString &text);
};

// AspeQt Time Server // Ray A.
class AspeCl: public SioDevice
{
    Q_OBJECT

public:
    AspeCl(SioWorker *worker): SioDevice(worker) {}
    void handleCommand(quint8 command, quint16 aux);

public slots:
    void gotNewSlot (int slot);                         // Ray A.
    void fileMounted (bool mounted);                    // Ray A.

signals:
    void findNewSlot (int startFrom, bool createOne);
    void mountFile (int no, const QString fileName);
    void toggleAutoCommit (int no);                     // Ray A.

};

#endif // MISCDEVICES_H
