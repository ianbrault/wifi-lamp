/*
** devicefinder.cpp
*/

#include <QBluetoothDeviceInfo>

#include "device_finder.h"
#include "device_handler.h"

DeviceFinder::DeviceFinder(DeviceHandler* handler, QObject* parent)
    : QObject(parent), m_handler(handler)
{
    m_discovery_agent = new QBluetoothDeviceDiscoveryAgent();
    m_discovery_agent->setLowEnergyDiscoveryTimeout(5000);

    connect(
        m_discovery_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
        this, &DeviceFinder::found_device);
    connect(
        m_discovery_agent,
        QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
        this, &DeviceFinder::scan_error);
    connect(
        m_discovery_agent, &QBluetoothDeviceDiscoveryAgent::finished,
        this, &DeviceFinder::scan_finished);
}

DeviceFinder::~DeviceFinder()
{
    delete m_discovery_agent;
    m_discovery_agent = nullptr;
}

void DeviceFinder::found_device(const QBluetoothDeviceInfo& dev)
{
    if (dev.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        qDebug() << "DeviceFinder: Low-energy device found";
        if (dev.name() == "LampSrv")
        {
            m_device = dev;
            m_handler->set_device(&m_device);
            qDebug() << "DeviceFinder: Lamp device found";
            // stop the scan once the device has been found
            m_discovery_agent->stop();
        }
    }
}

void DeviceFinder::scan_error(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        qWarning() << "DeviceFinder: ERROR: Bluetooth adapter is powered off";
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        qWarning() << "DeviceFinder: ERROR: device I/O resulted in an error";
    else
        qWarning() << "DeviceFinder: ERROR: an unknown error occurred";
}

void DeviceFinder::scan_finished()
{
    if (!m_device.isValid())
    {
        qWarning() << "DeviceFinder: ERROR: Lamp device not found";
        emit device_not_found();
    }
}

void DeviceFinder::start_search()
{
    qDebug() << "Searching for Low Energy devices";
    m_handler->set_device(nullptr);
    m_discovery_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

const QBluetoothDeviceInfo& DeviceFinder::device() const
{
    return m_device;
}
