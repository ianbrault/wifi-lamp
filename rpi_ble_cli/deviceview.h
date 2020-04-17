#ifndef DEVICEVIEW_H
#define DEVICEVIEW_H

#include <QPixmap>
#include <QWidget>

class DeviceFinder;
class DeviceHandler;
class QLabel;

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

    QPixmap m_connected;
    QPixmap m_disconnected;

    QLabel* m_connection_icon;
    QLabel* m_device_name;
    QLabel* m_device_mac;
};

#endif // DEVICEVIEW_H
