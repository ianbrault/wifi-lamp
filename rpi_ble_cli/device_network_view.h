#ifndef DEVICE_NETWORK_VIEW_H
#define DEVICE_NETWORK_VIEW_H

#include <QWidget>

class LineEdit;
class QGridLayout;
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
    void set_network_password(QString&&);
    void set_edit_mode(bool);

private:
    QGridLayout* m_text_layout;

    QLabel* m_network_ssid;
    QLabel* m_network_password;

    LineEdit* m_network_ssid_edit;
    LineEdit* m_network_password_edit;

    QString m_password;
};

#endif // DEVICE_NETWORK_VIEW_H
