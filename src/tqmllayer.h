#ifndef TQMLLAYER_H
#define TQMLLAYER_H

#include <QObject>
#include <QString>
#include <QList>

class TQMLLayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString listComPortsString READ GetSerialPortsString NOTIFY listComPortsChanged)
    Q_PROPERTY(QList<QString> listComPorts READ GetSerialPort NOTIFY listComPortsChanged)
public:
    explicit TQMLLayer(QObject *parent = nullptr);
    QString sComPorts;
    QString GetSerialPortsString();
    QList<QString> GetSerialPort();
    QList<QString> comPorts;
    Q_INVOKABLE void processGetPorts();
    Q_INVOKABLE void openPort(QString sPort);
signals:
    void listComPortsChanged();
};

#endif // TQMLLAYER_H
