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

class ApeTime: public SioDevice
{
    Q_OBJECT

public:
    ApeTime(SioWorker *worker): SioDevice(worker) {}
    void handleCommand(quint8 command, quint16 aux);
};
// AspeQt Time Server // Ray A.
class AspeCl: public SioDevice
{
    Q_OBJECT

public:
    AspeCl(SioWorker *worker): SioDevice(worker) {}
    void handleCommand(quint8 command, quint16 aux);
};

#endif // MISCDEVICES_H
