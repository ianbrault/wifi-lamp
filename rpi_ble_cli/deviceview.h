#ifndef DEVICEVIEW_H
#define DEVICEVIEW_H

#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

class DeviceFinder;
class DeviceHandler;
class QPushButton;

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget* parent = nullptr);
    virtual ~ClickableLabel();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent*);
};

class DeviceInfoView : public QWidget
{
    Q_OBJECT

public:
    DeviceInfoView(QWidget* parent = nullptr);
    virtual ~DeviceInfoView();

private:
    QPixmap m_connected;
    QPixmap m_disconnected;

    QLabel* m_connection_icon;
    QLabel* m_device_name;
    QLabel* m_device_mac;
};

class DevicePairView : public QWidget
{
    Q_OBJECT

public:
    DevicePairView(QWidget* parent = nullptr);
    virtual ~DevicePairView();

private:
    QPixmap m_pair_unpaired;
    QPixmap m_pair_connected;
    QPixmap m_pair_disconnected;

    QLabel* m_pair_icon;
    QLabel* m_pair_text;
};

class DeviceNetworkView : public QWidget
{
    Q_OBJECT

public:
    DeviceNetworkView(QWidget* parent = nullptr);
    virtual ~DeviceNetworkView();

private:
    QLabel* m_network_name;
    QLabel* m_network_password;
};

class DeviceView : public QWidget
{
    Q_OBJECT

public:
    DeviceView(QWidget* parent = nullptr);
    virtual ~DeviceView();

private:
    void setup_layout();

private:
    DeviceFinder*  m_finder;
    DeviceHandler* m_handler;

    DeviceInfoView*    m_device_info;
    DevicePairView*    m_device_pair;
    DeviceNetworkView* m_device_network;

    bool m_lamp_is_on = false;
    QPixmap m_lamp_on;
    QPixmap m_lamp_off;
    ClickableLabel* m_device_on_off;
};

#endif // DEVICEVIEW_H
