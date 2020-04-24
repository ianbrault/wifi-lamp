/*
** devicehandler.cpp
*/

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

#include "devicehandler.h"
#include "uuid.hpp"

DeviceHandler::DeviceHandler(QObject* parent)
    : QObject(parent)
{

}

DeviceHandler::~DeviceHandler()
{
    if (m_ctrl)
    {
        delete m_ctrl;
        m_ctrl = nullptr;
    }

    if (m_service)
    {
        delete m_service;
        m_service = nullptr;
    }
}

void DeviceHandler::set_device(QBluetoothDeviceInfo* dev)
{
    m_device = dev;

    // disconnect and delete old connection
    if (m_ctrl)
    {
        m_ctrl->disconnectFromDevice();
        delete m_ctrl;
        m_ctrl = nullptr;
    }

    if (m_device)
    {
        m_ctrl = QLowEnergyController::createCentral(*m_device, this);

        // don't connect to QLowEnergyController::serviceDiscovered, can just
        // check if the services returned by createServiceObject are valid
        connect(
            m_ctrl, &QLowEnergyController::discoveryFinished,
            this, &DeviceHandler::service_scan_done);
        connect(m_ctrl, &QLowEnergyController::connected, this, [this]() {
            qDebug() << "DeviceHandler: Connected to device, searching for service" << SVC_UUID;
            m_ctrl->discoverServices();
        });
        connect(m_ctrl, &QLowEnergyController::disconnected, this, []() {
            qDebug() << "DeviceHandler: Disconnected from device";
        });

        m_ctrl->connectToDevice();
    }
}

void DeviceHandler::service_scan_done()
{
    qDebug() << "DeviceHandler: Service scan done";

    // delete old service if available
    if (m_service)
    {
        delete m_service;
        m_service = nullptr;
    }

    m_service = m_ctrl->createServiceObject(QBluetoothUuid(SVC_UUID), this);
    if (!m_service)
        qWarning() << "DeviceHandler: ERROR: lamp service" << SVC_UUID << "not found";
    else
    {
        qDebug() << "DeviceHandler: found service" << SVC_UUID;

        connect(
            m_service, &QLowEnergyService::stateChanged,
            this, &DeviceHandler::service_state_changed);

        qDebug() << "DeviceHandler: searching for service" << SVC_UUID << "characteristics";
        m_service->discoverDetails();
    }
}

void DeviceHandler::service_state_changed(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::ServiceDiscovered:
        qDebug() << "DeviceHandler: Service discovered, retrieving characteristics";
        add_characteristics();
        break;
    default:
        break;
    }
}

void DeviceHandler::add_characteristics()
{
    m_dev_mac = m_service->characteristic(QBluetoothUuid(DEV_MAC_UUID));
    if (!m_dev_mac.isValid())
        qWarning() << "DeviceHandler: MAC address characteristic" << DEV_MAC_UUID << "is invalid";
    else
        qDebug() << "DeviceHandler: found MAC address characteristic" << DEV_MAC_UUID;

    m_dev_name = m_service->characteristic(QBluetoothUuid(DEV_NAME_UUID));
    if (!m_dev_name.isValid())
        qWarning() << "DeviceHandler: device name characteristic" << DEV_NAME_UUID << "is invalid";
    else
        qDebug() << "DeviceHandler: found device name characteristic" << DEV_NAME_UUID;

    // TODO: emit signal
}
