#ifndef DEVICEFINDER_H
#define DEVICEFINDER_H

#include <QBluetoothDeviceDiscoveryAgent>

class DeviceHandler;
class QBluetoothDeviceInfo;

class DeviceFinder : public QObject
{
    Q_OBJECT

public:
    DeviceFinder(DeviceHandler* handler, QObject* parent = nullptr);
    virtual ~DeviceFinder();

public:
    void start_search();
    const QBluetoothDeviceInfo& device() const;

private slots:
    void found_device(const QBluetoothDeviceInfo&);
    void scan_error(QBluetoothDeviceDiscoveryAgent::Error);
    void scan_finished();

private:
    QBluetoothDeviceDiscoveryAgent* m_discovery_agent;
    QBluetoothDeviceInfo            m_device;
    DeviceHandler*                  m_handler;
};

#endif // DEVICEFINDER_H
