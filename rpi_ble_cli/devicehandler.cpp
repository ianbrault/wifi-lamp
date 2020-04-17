/*
** devicehandler.cpp
*/

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

#include "devicehandler.h"

const static QString svc_uuid("00000001-dead-fade-cafe-204080160320");
const static QString mac_uuid("00000002-dead-fade-cafe-204080160320");

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
            qDebug() << "DeviceHandler: Connected to device, searching for service " << svc_uuid;
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

    m_service = m_ctrl->createServiceObject(QBluetoothUuid(svc_uuid), this);
    if (!m_service)
        qWarning() << "DeviceHandler: ERROR: lamp service " << svc_uuid << " not found";
    else
    {
        qDebug() << "DeviceHandler: found service " << svc_uuid;

        connect(
            m_service, &QLowEnergyService::stateChanged,
            this, &DeviceHandler::service_state_changed);

        qDebug() << "DeviceHandler: searching for service " << svc_uuid << " characteristics";
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
    m_mac_addr = m_service->characteristic(QBluetoothUuid(mac_uuid));
    if (!m_mac_addr.isValid())
        qWarning() << "DeviceHandler: MAC address characteristic " << mac_uuid << " is invalid";
    else
        qDebug() << "DeviceHandler: found MAC address characteristic " << mac_uuid;

    // TODO: emit signal
}
