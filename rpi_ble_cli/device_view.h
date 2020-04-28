#ifndef DEVICE_VIEW_H
#define DEVICE_VIEW_H

#include <QPixmap>
#include <QWidget>

class ClickableLabel;
class DeviceFinder;
class DeviceHandler;
class DeviceInfoView;
class QBluetoothDeviceInfo;
class QLabel;
class QPushButton;
class Spinner;

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
    void search_for_device();

private slots:
    void device_found();
    void device_not_found();

private:
    DeviceFinder*  m_finder;
    DeviceHandler* m_handler;

    Spinner* m_spinner;
    bool m_searching = false;
    bool m_device_found = false;

    DeviceInfoView*    m_device_info;
    DevicePairView*    m_device_pair;
    DeviceNetworkView* m_device_network;

    bool m_lamp_is_on = false;
    QPixmap m_lamp_on;
    QPixmap m_lamp_off;
    ClickableLabel* m_device_on_off;
};

#endif // DEVICE_VIEW_H
