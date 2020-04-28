#ifndef DEVICE_INFO_VIEW_H
#define DEVICE_INFO_VIEW_H

#include <QPixmap>
#include <QWidget>

class QLabel;

class DeviceInfoView : public QWidget
{
    Q_OBJECT

public:
    DeviceInfoView(QWidget* parent = nullptr);
    virtual ~DeviceInfoView();

public:
    void set_device_info(std::string&&, std::string&&);

private:
    void set_name(std::string&&);
    void set_mac(std::string&&);

private:
    QPixmap m_connected;
    QPixmap m_disconnected;

    QLabel* m_connection_icon;
    QLabel* m_device_name;
    QLabel* m_device_mac;
};


#endif // DEVICE_INFO_VIEW_H
