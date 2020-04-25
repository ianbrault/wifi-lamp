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
    std::string read_device_mac();
    std::string read_device_name();

private:
    void service_scan_done();
    void service_state_changed(QLowEnergyService::ServiceState);
    void add_characteristics();

signals:
    void device_found();
    void device_characteristic_error();

private:
    QLowEnergyController* m_ctrl    = nullptr;
    QBluetoothDeviceInfo* m_device  = nullptr;
    QLowEnergyService*    m_service = nullptr;

    QLowEnergyCharacteristic m_dev_mac;
    QLowEnergyCharacteristic m_dev_name;
};

#endif // DEVICEHANDLER_H
