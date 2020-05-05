/*
** main.cpp
*/

#include <iomanip>
#include <sstream>

#include <QCoreApplication>
#include <QDebug>
#include <QLowEnergyAdvertisingData>
#include <QLowEnergyAdvertisingParameters>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyCharacteristicData>
#include <QLowEnergyController>
#include <QLowEnergyDescriptorData>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>
#include <QScopedPointer>

#include "connection_handler.hpp"
#include "device_info.hpp"
#include "uuid.hpp"

const auto read_only  = QLowEnergyCharacteristic::Read;
const auto read_write = QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Write;

static QLowEnergyServiceData device_info_service;
static QLowEnergyServiceData device_network_service;

static QLowEnergyCharacteristicData dev_name_char;
static QLowEnergyCharacteristicData dev_mac_char;
static QLowEnergyCharacteristicData dev_ptnr_char;
static QLowEnergyCharacteristicData nwk_stat_char;
static QLowEnergyCharacteristicData nwk_ssid_char;
static QLowEnergyCharacteristicData nwk_pass_char;

static std::string uuid_to_string(const QBluetoothUuid& uuid)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(2)
           << "0x" << std::hex << uuid.toUInt16();

    return stream.str();
}

static void define_device_info_service(const DeviceInfo& dev_info)
{
    qDebug() << "Creating device information service"
             << uuid_to_string(DeviceInfoService).c_str();

    device_info_service.setType(QLowEnergyServiceData::ServiceTypePrimary);
    device_info_service.setUuid(DeviceInfoService);

    // define device name characteristic
    qDebug() << "Creating device name characteristic"
             << uuid_to_string(DeviceNameChar).c_str();

    dev_name_char.setUuid(DeviceNameChar);
    dev_name_char.setValue(QByteArray::fromStdString(dev_info.name()));
    dev_name_char.setValueLength(0, 64);
    dev_name_char.setProperties(read_write);

    if (!dev_name_char.isValid())
        qWarning() << "Device name characteristic is invalid";

    // define device MAC address characteristic
    qDebug() << "Creating device MAC characteristic"
             << uuid_to_string(DeviceMacChar).c_str();

    dev_mac_char.setUuid(DeviceMacChar);
    dev_mac_char.setValue(dev_info.mac_bytes());
    dev_mac_char.setValueLength(6, 6);
    dev_mac_char.setProperties(read_only);

    if (!dev_mac_char.isValid())
        qWarning() << "Device MAC characteristic is invalid";

    device_info_service.addCharacteristic(dev_name_char);
    device_info_service.addCharacteristic(dev_mac_char);

    if (!device_info_service.isValid())
        qWarning() << "Device info service is invalid";
}

static void define_device_network_service(const DeviceInfo& dev_info)
{
    qDebug() << "Creating device network service"
             << uuid_to_string(DeviceNetworkService).c_str();

    auto ssid = QByteArray::fromStdString(dev_info.network_ssid());
    auto password = QByteArray::fromStdString(dev_info.network_password());

    device_network_service.setType(QLowEnergyServiceData::ServiceTypePrimary);
    device_network_service.setUuid(DeviceNetworkService);

    // define network SSID characteristic
    qDebug() << "Creating network SSID characteristic"
             << uuid_to_string(NetworkSSIDChar).c_str();

    nwk_ssid_char.setUuid(NetworkSSIDChar);
    nwk_ssid_char.setValue(ssid);
    nwk_ssid_char.setValueLength(0, 64);
    nwk_ssid_char.setProperties(read_write);

    if (!nwk_ssid_char.isValid())
        qWarning() << "Network SSID characteristic is invalid";

    // define network password characteristic
    qDebug() << "Creating network password characteristic"
             << uuid_to_string(NetworkPasswordChar).c_str();

    nwk_pass_char.setUuid(NetworkPasswordChar);
    nwk_pass_char.setValue(password);
    nwk_pass_char.setValueLength(0, 64);
    nwk_pass_char.setProperties(read_write);

    if (!nwk_pass_char.isValid())
        qWarning() << "Network password characteristic is invalid";

    device_network_service.addCharacteristic(nwk_ssid_char);
    device_network_service.addCharacteristic(nwk_pass_char);

    if (!device_network_service.isValid())
        qWarning() << "Device network service is invalid";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DeviceInfo device_info;

    qDebug() << "device name:" << device_info.name().c_str();
    qDebug() << "device MAC address:" << device_info.mac_string().c_str();
    qDebug() << "network SSID:" << device_info.network_ssid().c_str();
    qDebug() << "network password:" << device_info.network_password().c_str();

    // set up BLE advertising data
    QLowEnergyAdvertisingData ad_data;
    ad_data.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    ad_data.setLocalName("LampSrv");
    ad_data.setServices(QList<QBluetoothUuid>()
        << DeviceInfoService
        << DeviceNetworkService);

    // define services
    define_device_info_service(device_info);
    define_device_network_service(device_info);

    // initialize the BLE peripheral controller and start advertising
    qDebug() << "Initializing BLE peripheral controller & advertising data";
    QScopedPointer<QLowEnergyController> ble_ctrl(QLowEnergyController::createPeripheral());

    QScopedPointer<QLowEnergyService> info_service(ble_ctrl->addService(device_info_service));
    QScopedPointer<QLowEnergyService> nwk_service(ble_ctrl->addService(device_network_service));
    ble_ctrl->startAdvertising(QLowEnergyAdvertisingParameters(), ad_data, ad_data);

    // set up connection handlers
    ConnectionHandler conn_handler;
    QObject::connect(
        ble_ctrl.data(), &QLowEnergyController::connected,
        &conn_handler, &ConnectionHandler::client_connected);
    QObject::connect(
        ble_ctrl.data(), &QLowEnergyController::disconnected,
        &conn_handler, &ConnectionHandler::client_disconnected);
    QObject::connect(
        ble_ctrl.data(), &QLowEnergyController::stateChanged,
        &conn_handler, &ConnectionHandler::state_changed);

    // set up reconnect handler
    auto reconnect = [&ble_ctrl, ad_data, &info_service, &nwk_service]() {
        info_service.reset(ble_ctrl->addService(device_info_service));
        nwk_service.reset(ble_ctrl->addService(device_network_service));

        if (!info_service.isNull() && !nwk_service.isNull())
            ble_ctrl->startAdvertising(
                QLowEnergyAdvertisingParameters(), ad_data, ad_data);
    };
    QObject::connect(ble_ctrl.data(), &QLowEnergyController::disconnected, reconnect);

    // set up service handlers to watch for characteristic writes
    auto write_handler = [&device_info](const QLowEnergyCharacteristic& c, const QByteArray& v) {
        auto vstr = v.toStdString();
        if (c.uuid() == DeviceNameChar)
            device_info.set_name(std::move(vstr));
        else if (c.uuid() == NetworkSSIDChar)
            device_info.set_network_ssid(std::move(vstr));
        else if (c.uuid() == NetworkPasswordChar)
            device_info.set_network_password(std::move(vstr));
    };
    QObject::connect(info_service.data(), &QLowEnergyService::characteristicWritten, write_handler);
    QObject::connect(nwk_service.data(), &QLowEnergyService::characteristicWritten, write_handler);

    return a.exec();
}
