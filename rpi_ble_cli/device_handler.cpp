/*
** devicehandler.cpp
*/

#include "device_handler.h"
#include "uuid.hpp"

#include <iomanip>
#include <sstream>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

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
            this,      &DeviceHandler::service_state_changed);
        connect(
            m_service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
            this,      &DeviceHandler::service_error);

        m_service->discoverDetails();
    }
}

void DeviceHandler::service_state_changed(QLowEnergyService::ServiceState s)
{
    switch (s)
    {
        case QLowEnergyService::InvalidService:
            qDebug() << "DeviceHandler: service is invalid";
            break;
        case QLowEnergyService::DiscoveryRequired:
            qDebug() << "DeviceHandler: service discovery is required";
            break;
        case QLowEnergyService::DiscoveringServices:
            qDebug() << "DeviceHandler: services are being discovered";
            break;
        case QLowEnergyService::ServiceDiscovered:
            qDebug() << "DeviceHandler: service discovered, retrieving characteristics";
            add_characteristics();
            break;
        case QLowEnergyService::LocalService:
            qDebug() << "DeviceHandler: service is associated with a controller object in the peripheral role";
            break;
        default:
            break;
    }
}

void DeviceHandler::service_error(QLowEnergyService::ServiceError e)
{
    switch (e)
    {
        case QLowEnergyService::OperationError:
            qWarning() << "DeviceHandler: an operation was attempted while the service was not ready";
            break;
        case QLowEnergyService::CharacteristicReadError:
            qWarning() << "DeviceHandler: an attempt to read a characteristic value failed";
            break;
        case QLowEnergyService::CharacteristicWriteError:
            qWarning() << "DeviceHandler: an attempt to write a characteristic value failed";
            break;
        case QLowEnergyService::UnknownError:
            qWarning() << "DeviceHandler: unknown service error";
            break;
        case QLowEnergyService::NoError:
        default:
            break;
    }
}

void DeviceHandler::add_characteristics()
{
    qDebug() << "DeviceHandler: find MAC address characteristic" << DEV_MAC_UUID;
    m_dev_mac = m_service->characteristic(QBluetoothUuid(DEV_MAC_UUID));
    if (!m_dev_mac.isValid())
    {
        qWarning() << "DeviceHandler: MAC address characteristic is invalid";
        emit device_characteristic_error();
        return;
    }

    qDebug() << "DeviceHandler: find device name characteristic" << DEV_NAME_UUID;
    m_dev_name = m_service->characteristic(QBluetoothUuid(DEV_NAME_UUID));
    if (!m_dev_name.isValid())
    {
        qWarning() << "DeviceHandler: device name characteristic is invalid";
        emit device_characteristic_error();
        return;
    }

    qDebug() << "DeviceHandler: find network SSID characteristic" << NWK_SSID_UUID;
    m_nwk_ssid = m_service->characteristic(QBluetoothUuid(NWK_SSID_UUID));
    if (!m_nwk_ssid.isValid())
    {
        qWarning() << "DeviceHandler: network SSID characteristic is invalid";
        emit device_characteristic_error();
        return;
    }

    qDebug() << "DeviceHandler: find network password characteristic" << NWK_PWD_UUID;
    m_nwk_pass = m_service->characteristic(QBluetoothUuid(NWK_PWD_UUID));
    if (!m_nwk_pass.isValid())
    {
        qWarning() << "DeviceHandler: network password characteristic is invalid";
        emit device_characteristic_error();
        return;
    }

    emit device_found();
}

std::string DeviceHandler::read_device_mac()
{
    if (!m_dev_mac.isValid())
        return std::string();

    auto mac_bytes = m_dev_mac.value();

    std::stringstream stream;
    stream << std::setfill('0') << std::setw(2);
    for (auto i = 0; i < 6; i++)
    {
        auto byte = ((int) mac_bytes.at(i)) & 0xff;
        stream << std::hex << byte;
        if (i != 5)
            stream << ":";
    }

    return stream.str();
}

std::string DeviceHandler::read_device_name()
{
    if (!m_dev_name.isValid())
        return std::string();

    auto name_bytes = m_dev_name.value();
    return name_bytes.toStdString();
}

std::string DeviceHandler::read_network_ssid()
{
    if (!m_nwk_ssid.isValid())
        return std::string();

    auto ssid_bytes = m_nwk_ssid.value();
    return ssid_bytes.toStdString();
}

std::string DeviceHandler::read_network_password()
{
    if (!m_nwk_pass.isValid())
        return std::string();

    auto pass_bytes = m_nwk_pass.value();
    return pass_bytes.toStdString();
}
