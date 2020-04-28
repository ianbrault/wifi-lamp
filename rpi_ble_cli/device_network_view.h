#ifndef DEVICE_NETWORK_VIEW_H
#define DEVICE_NETWORK_VIEW_H

#include <QWidget>

class QLabel;

class DeviceNetworkView : public QWidget
{
    Q_OBJECT

public:
    DeviceNetworkView(QWidget* parent = nullptr);
    virtual ~DeviceNetworkView();

public:
    void set_network_ssid(std::string&&);
    void set_network_password(std::string&&);

private:
    QLabel* m_network_ssid;
    QLabel* m_network_password;
};

#endif // DEVICE_NETWORK_VIEW_H
