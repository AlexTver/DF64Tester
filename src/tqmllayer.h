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
    Q_PROPERTY(QString deviceInfoString READ GetDeviceInfoString NOTIFY infoStringChanged)
    Q_PROPERTY(QString devNextState READ GetDevNextStateString NOTIFY stateChanged)
    Q_PROPERTY(QString dataString READ GetDataString NOTIFY dataStringChanged)
public:
    explicit TQMLLayer(QObject *parent = nullptr);
    QString sComPorts;
    QString GetSerialPortsString();
    QList<QString> GetSerialPort();
    QList<QString> comPorts;
    Q_INVOKABLE void processGetPorts();
    Q_INVOKABLE void openPort(QString sPort);
    Q_INVOKABLE void changeState();
    QString GetDeviceInfoString();
    QString GetDevNextStateString();
    QString GetDataString();
    void dataChanged();
signals:
    void listComPortsChanged();
    void infoStringChanged();
    void dataStringChanged();
    void stateChanged();
};

#endif // TQMLLAYER_H
