/*
** devicehandler.cpp
*/

#include "device_handler.h"
#include "uuid.hpp"

#include <iomanip>
#include <sstream>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

static std::string uuid_to_string(const QBluetoothUuid& uuid)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(2)
           << "0x" << std::hex << uuid.toUInt16();

    return stream.str();
}

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

    if (m_info_service)
    {
        delete m_info_service;
        m_info_service = nullptr;
    }

    if (m_network_service)
    {
        delete m_network_service;
        m_network_service = nullptr;
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
            qDebug() << "DeviceHandler: connected to device, searching for services";
            m_ctrl->discoverServices();
        });
        connect(m_ctrl, &QLowEnergyController::disconnected, this, []() {
            qDebug() << "DeviceHandler: disconnected from device";
        });

        m_info_service_done = false;
        m_network_service_done = false;
        m_ctrl->connectToDevice();
    }
}

void DeviceHandler::service_scan_done()
{
    qDebug() << "DeviceHandler: Service scan done";

    // delete old service(s) if available
    if (m_info_service)
    {
        delete m_info_service;
        m_info_service = nullptr;
    }
    if (m_network_service)
    {
        delete m_network_service;
        m_network_service = nullptr;
    }

    // when both services have been fully discovered, emit a signal
    auto service_done = [this](const QBluetoothUuid& svc_uuid) mutable {
        if (svc_uuid == DeviceInfoService)
            m_info_service_done = true;
        else if (svc_uuid == DeviceNetworkService)
            m_network_service_done = true;

        if (m_info_service_done && m_network_service_done)
            emit device_found();
    };
    connect(this, &DeviceHandler::service_discovered, service_done);

    // discover device information service
    m_info_service = m_ctrl->createServiceObject(DeviceInfoService, this);
    if (!m_info_service)
    {
        qWarning() << "DeviceHandler: ERROR: device information service"
                   << uuid_to_string(DeviceInfoService).c_str() << "not found";
    }
    else
    {
        qDebug() << "DeviceHandler: found service"
                 << uuid_to_string(DeviceInfoService).c_str();

        // bind info service UUID to slot handlers
        connect(
            m_info_service, &QLowEnergyService::stateChanged,
            [&](const QLowEnergyService::ServiceState& state) {
                service_state_changed(DeviceInfoService, state);
            });
        connect(
            m_info_service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
            [&](const QLowEnergyService::ServiceError& error) {
                service_error(DeviceInfoService, error);
            });

        m_info_service->discoverDetails();
    }

    // discover device network service
    m_network_service = m_ctrl->createServiceObject(DeviceNetworkService, this);
    if (!m_network_service)
    {
        qWarning() << "DeviceHandler: ERROR: device network service"
                   << uuid_to_string(DeviceNetworkService).c_str() << "not found";
    }
    else
    {
        qDebug() << "DeviceHandler: found service"
                 << uuid_to_string(DeviceNetworkService).c_str();

        // bind network service UUID to slot handlers
        connect(
            m_network_service, &QLowEnergyService::stateChanged,
            [&](const QLowEnergyService::ServiceState& state) {
                service_state_changed(DeviceNetworkService, state);
            });
        connect(
            m_network_service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
            [&](const QLowEnergyService::ServiceError& error) {
                service_error(DeviceNetworkService, error);
            });

        m_network_service->discoverDetails();
    }
}

void DeviceHandler::service_state_changed(const QBluetoothUuid& service_uuid, QLowEnergyService::ServiceState s)
{
    auto log_state = [&](const char* msg) {
        qDebug() << "DeviceHandler: service"
                 << uuid_to_string(service_uuid).c_str()
                 << msg;
    };

    switch (s)
    {
        case QLowEnergyService::InvalidService:
            log_state("is invalid");
            break;
        case QLowEnergyService::DiscoveryRequired:
            log_state("discovery is required");
            break;
        case QLowEnergyService::DiscoveringServices:
            log_state("discovering services");
            break;
        case QLowEnergyService::ServiceDiscovered:
            log_state("discovered");
            /*
            (service_uuid == DeviceInfoService)
                ? add_info_characteristics()
                : add_network_characteristics();
            */
            emit service_discovered(service_uuid);
            break;
        case QLowEnergyService::LocalService:
            log_state("is associated with a controller object in the peripheral role");
            break;
        default:
            break;
    }
}

void DeviceHandler::service_error(const QBluetoothUuid& service_uuid, QLowEnergyService::ServiceError e)
{
    auto log_error = [&](const char* msg) {
        qWarning() << "DeviceHandler: service"
                 << uuid_to_string(service_uuid).c_str()
                 << msg;
    };

    switch (e)
    {
        case QLowEnergyService::OperationError:
            log_error("an operation was attempted while the service was not ready");
            break;
        case QLowEnergyService::CharacteristicReadError:
            log_error("an attempt to read a characteristic value failed");
            break;
        case QLowEnergyService::CharacteristicWriteError:
            log_error("an attempt to write a characteristic value failed");
            break;
        case QLowEnergyService::DescriptorReadError:
            log_error("an attempt to read a descriptor value failed");
            break;
        case QLowEnergyService::DescriptorWriteError:
            log_error("an attempt to write a descriptor value failed");
            break;
        case QLowEnergyService::UnknownError:
            log_error("unknown  error");
            break;
        case QLowEnergyService::NoError:
        default:
            break;
    }
}

void DeviceHandler::add_info_characteristics()
{
    bool valid = true;

    qDebug() << "DeviceHandler: find device name characteristic"
             << uuid_to_string(DeviceNameChar).c_str();
    m_dev_name = m_info_service->characteristic(DeviceNameChar);
    valid = valid && m_dev_name.isValid();
    if (!m_dev_name.isValid())
    {
        qWarning() << "DeviceHandler: device name characteristic is invalid";
        emit device_characteristic_error();
    }

    qDebug() << "DeviceHandler: find MAC address characteristic"
             << uuid_to_string(DeviceMacChar).c_str();
    m_dev_mac = m_info_service->characteristic(DeviceMacChar);
    valid = valid && m_dev_mac.isValid();
    if (!m_dev_mac.isValid())
    {
        qWarning() << "DeviceHandler: MAC address characteristic is invalid";
        emit device_characteristic_error();
    }

    /*
    if (valid)
        emit service_discovered(DeviceInfoService);
    */
}

void DeviceHandler::add_network_characteristics()
{
    bool valid = true;

    qDebug() << "DeviceHandler: find network SSID characteristic"
             << uuid_to_string(NetworkSSIDChar).c_str();
    m_nwk_ssid = m_network_service->characteristic(NetworkSSIDChar);
    valid = valid && m_nwk_ssid.isValid();
    if (!m_nwk_ssid.isValid())
    {
        qWarning() << "DeviceHandler: network SSID characteristic is invalid";
        emit device_characteristic_error();
    }

    qDebug() << "DeviceHandler: find network password characteristic"
             << uuid_to_string(NetworkPasswordChar).c_str();
    m_nwk_pass = m_network_service->characteristic(NetworkPasswordChar);
    valid = valid && m_nwk_pass.isValid();
    if (!m_nwk_pass.isValid())
    {
        qWarning() << "DeviceHandler: network password characteristic is invalid";
        emit device_characteristic_error();
    }

    /*
    if (valid)
        emit service_discovered(DeviceNetworkService);
    */
}

std::string DeviceHandler::read_device_mac()
{
    /*
    if (!m_dev_mac.isValid())
        return std::string();
    */
    auto c = m_info_service->characteristic(DeviceMacChar);

    // auto mac_bytes = m_dev_mac.value();
    auto mac_bytes = c.value();

    std::stringstream stream;
    stream << std::setfill('0') << std::setw(2);
    for (auto i = 0; i < mac_bytes.length(); i++)
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
    /*
    if (!m_dev_name.isValid())
        return std::string();
    */
    auto c = m_info_service->characteristic(DeviceNameChar);

    qDebug() << "DEBUG: device name properties:" << c.properties();

    // auto name_bytes = m_dev_name.value();
    // return name_bytes.toStdString();
    return c.value().toStdString();
}

std::string DeviceHandler::read_network_ssid()
{
    /*
    if (!m_nwk_ssid.isValid())
        return std::string();

    auto ssid_bytes = m_nwk_ssid.value();
    return ssid_bytes.toStdString();
    */
    return "TEST NETWORK";
}

std::string DeviceHandler::read_network_password()
{
    /*
    if (!m_nwk_pass.isValid())
        return std::string();

    auto pass_bytes = m_nwk_pass.value();
    return pass_bytes.toStdString();
    */
    return "password123";
}

void DeviceHandler::write_device_name(QString&& name)
{
    if (!m_dev_name.isValid())
        return;

    m_info_service->writeCharacteristic(
        m_dev_name, name.toUtf8(), QLowEnergyService::WriteWithoutResponse);
}

void DeviceHandler::write_network_ssid(QString&& ssid)
{
    if (!m_nwk_ssid.isValid())
        return;

    m_network_service->writeCharacteristic(
        m_nwk_ssid, ssid.toUtf8(), QLowEnergyService::WriteWithoutResponse);
}

void DeviceHandler::write_network_password(QString&& password)
{
    if (!m_nwk_pass.isValid())
        return;

    m_network_service->writeCharacteristic(
        m_nwk_pass, password.toUtf8(), QLowEnergyService::WriteWithoutResponse);
}
