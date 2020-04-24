/*
** main.cpp
*/

#include <QBluetoothUuid>
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DeviceInfo device_info;

    // set up BLE advertising data
    QLowEnergyAdvertisingData ad_data;
    ad_data.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    ad_data.setLocalName("LampSrv");
    ad_data.setServices(QList<QBluetoothUuid>() << QBluetoothUuid(SVC_UUID));

    /*
    ** define BLE characteristics
    */

    // define device MAC address characteristic data
    qDebug() << "Adding device MAC characteristic data " << DEV_MAC_UUID;

    QLowEnergyCharacteristicData mac_char;
    mac_char.setUuid(QBluetoothUuid(DEV_MAC_UUID));
    mac_char.setValue(device_info.mac_bytes());
    mac_char.setValueLength(6, 6);
    mac_char.setProperties(QLowEnergyCharacteristic::Read);

    // define device name characteristic data
    qDebug() << "Adding device name characteristic data " << DEV_NAME_UUID;

    QLowEnergyCharacteristicData name_char;
    name_char.setUuid(QBluetoothUuid(DEV_NAME_UUID));
    name_char.setValue(QByteArray::fromStdString(device_info.name()));
    name_char.setValueLength(0, 64);
    // TODO: should be writable as well
    name_char.setProperties(QLowEnergyCharacteristic::Read);

    /*
    ** define BLE service
    */
    qDebug() << "Creating BLE service " << SVC_UUID;

    QLowEnergyServiceData svc_data;
    svc_data.setType(QLowEnergyServiceData::ServiceTypePrimary);
    svc_data.setUuid(QBluetoothUuid(SVC_UUID));

    svc_data.addCharacteristic(mac_char);
    svc_data.addCharacteristic(name_char);

    /*
    ** initialize the BLE peripheral controller and start advertising
    */

    qDebug() << "Initializing BLE peripheral controller";
    const QScopedPointer<QLowEnergyController> ble_ctrl(QLowEnergyController::createPeripheral());

    qDebug() << "Begin advertising data";
    QScopedPointer<QLowEnergyService> service(ble_ctrl->addService(svc_data));
    ble_ctrl->startAdvertising(QLowEnergyAdvertisingParameters(), ad_data, ad_data);

    /*
    ** set up connection handlers
    */

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

    auto reconnect = [&ble_ctrl, ad_data, &service, svc_data]()
    {
        service.reset(ble_ctrl->addService(svc_data));
        if (!service.isNull())
            ble_ctrl->startAdvertising(
                QLowEnergyAdvertisingParameters(), ad_data, ad_data);
    };
    QObject::connect(ble_ctrl.data(), &QLowEnergyController::disconnected, reconnect);

    return a.exec();
}
