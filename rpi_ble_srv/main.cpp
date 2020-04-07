/*
** main.cpp
*/

#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QLowEnergyAdvertisingData>
#include <QLowEnergyAdvertisingParameters>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyCharacteristicData>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>
#include <QScopedPointer>

#include "uuid.hpp"

static const char mac_addr[] = {
    '\xfa', '\xde', '\xca', '\xfe', '\x42', '\x00'
};

/*
class ConnectionHandler : public QObject
{
    Q_OBJECT

public:
    ConnectionHandler();

public slots:
    void client_connected();
    void client_disconnected();
};
*/

void client_connected()
{
    qDebug() << "Client connected";
}

void client_disconnected()
{
    qDebug() << "Client disconnected";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Initializing BLE peripheral controller";
    const QScopedPointer<QLowEnergyController> ble_ctrl(QLowEnergyController::createPeripheral());

    // define BLE service
    QLowEnergyServiceData svc_data;
    svc_data.setType(QLowEnergyServiceData::ServiceTypePrimary);
    svc_data.setUuid(SVC_UUID);

    // define MAC address characteristic data
    qDebug() << "Adding device MAC characteristic data";

    QLowEnergyCharacteristicData mac_char;
    mac_char.setUuid(MAC_UUID);
    mac_char.setProperties(QLowEnergyCharacteristic::Read);
    // TODO: need to set the MAC address
    mac_char.setValue(QByteArray::fromRawData(mac_addr, 6));

    svc_data.addCharacteristic(mac_char);

    // set up BLE advertising data
    QLowEnergyAdvertisingData ad_data;
    ad_data.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    ad_data.setLocalName("LampSrv");
    ad_data.setServices(QList<QBluetoothUuid>() << SVC_UUID);

    // start advertising
    qDebug() << "Begin advertising data";
    QScopedPointer<QLowEnergyService> svc(ble_ctrl->addService(svc_data));
    ble_ctrl->startAdvertising(QLowEnergyAdvertisingParameters(), ad_data, ad_data);

    // ConnectionHandler conn_handler;
    QObject::connect(
        ble_ctrl.data(), &QLowEnergyController::connected,
        client_connected);
    QObject::connect(
        ble_ctrl.data(), &QLowEnergyController::disconnected,
        client_disconnected);

    return a.exec();
}
