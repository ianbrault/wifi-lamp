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
    std::string read_network_ssid();
    std::string read_network_password();

private:
    void service_scan_done();
    void service_state_changed(const QBluetoothUuid&, QLowEnergyService::ServiceState);
    void service_error(const QBluetoothUuid&, QLowEnergyService::ServiceError);
    void add_characteristics();
    bool add_info_characteristics();
    bool add_network_characteristics();

signals:
    void device_found();
    void device_characteristic_error();
    void service_discovered(const QBluetoothUuid&);

private:
    QLowEnergyController* m_ctrl    = nullptr;
    QBluetoothDeviceInfo* m_device  = nullptr;

    QLowEnergyService*    m_info_service    = nullptr;
    QLowEnergyService*    m_network_service = nullptr;

    QLowEnergyCharacteristic m_dev_mac;
    QLowEnergyCharacteristic m_dev_name;
    QLowEnergyCharacteristic m_nwk_ssid;
    QLowEnergyCharacteristic m_nwk_pass;
};

#endif // DEVICEHANDLER_H
