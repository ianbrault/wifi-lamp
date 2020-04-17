#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H

#include <QLowEnergyCharacteristic>
#include <QLowEnergyService>

class QBluetoothDeviceInfo;
class QLowEnergyController;

class DeviceHandler : public QObject
{
    Q_OBJECT

public:
    DeviceHandler(QObject* parent = nullptr);
    virtual ~DeviceHandler();

public:
    void set_device(QBluetoothDeviceInfo*);
private:
    void service_scan_done();
    void service_state_changed(QLowEnergyService::ServiceState);
    void add_characteristics();

private:
    QLowEnergyController* m_ctrl    = nullptr;
    QBluetoothDeviceInfo* m_device  = nullptr;
    QLowEnergyService*    m_service = nullptr;

    QLowEnergyCharacteristic m_mac_addr;
};

#endif // DEVICEHANDLER_H
