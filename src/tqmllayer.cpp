#include <QSerialPortInfo>

#include "tqmllayer.h"
#include "fn_log.h"

TQMLLayer::TQMLLayer(QObject *parent)
    : QObject{parent}
{
    sComPorts = "Список обнаруженных COM-портов";
}

QString TQMLLayer::GetSerialPortsString()
{
    sComPorts = "Список обнаруженных COM-портов:";
    const auto comPortsList = comPorts;
    for (const QString &portID : comPortsList) {
        sComPorts += "\n" + portID;
    }
    return sComPorts;
}

QList<QString> TQMLLayer::GetSerialPort()
{
    LogDebug() << "Get serial port list call";
    return comPorts;
}

void TQMLLayer::processGetPorts()
{
    LogDebug() << "Get serial ports call";
    comPorts.clear();
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    if (serialPortInfos.size() > 0) {
        LogDebug() << "Serial ports detected:";
        for (const QSerialPortInfo &portInfo : serialPortInfos) {
            QString sTmp;
            sTmp = sTmp + "\n"
                   + "Port:" + portInfo.portName() + "\n"
                   + "Location:" + portInfo.systemLocation() + "\n"
                   + "Description:" + portInfo.description() + "\n"
                   + "Manufacturer:" + portInfo.manufacturer() + "\n"
                   + "Serial number:" + portInfo.serialNumber() + "\n"
                   + "Vendor Identifier:"
                   + (portInfo.hasVendorIdentifier()
                          ? QByteArray::number(portInfo.vendorIdentifier(), 16).toHex()
                          : QByteArray().toHex()) + "\n"
                   + "Product Identifier:"
                   + (portInfo.hasProductIdentifier()
                          ? QByteArray::number(portInfo.productIdentifier(), 16).toHex()
                          : QByteArray().toHex());
            comPorts.append(portInfo.systemLocation());
            LogDebug() << sTmp.toStdString();
        }
    } else {
        LogDebug() << "No serial ports find";
    }
    emit listComPortsChanged();
}

void TQMLLayer::openPort(QString sPort)
{
    LogInfo() << "Open port '" << sPort.toStdString() << "'";
}


