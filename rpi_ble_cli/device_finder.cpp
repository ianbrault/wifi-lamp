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
        if (dev.name() == "LampSrv")
            m_device = dev;
    }
}

void DeviceFinder::scan_error(QBluetoothDeviceDiscoveryAgent::Error error)
{
    qWarning() << "DeviceFinder ERROR:" << m_discovery_agent->errorString();
}

void DeviceFinder::scan_finished()
{
    if (!m_device.isValid())
    {
        qWarning() << "DeviceFinder: ERROR: Lamp device not found";
        emit device_not_found();
    }
    else
    {
        qDebug() << "DeviceFinder: Lamp device found";
        m_handler->set_device(&m_device);
    }
}

void DeviceFinder::start_search()
{
    qDebug() << "Searching for Low Energy devices";
    m_handler->set_device(nullptr);
    m_discovery_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}
