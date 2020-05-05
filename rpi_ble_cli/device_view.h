#ifndef DEVICE_VIEW_H
#define DEVICE_VIEW_H

#include <QPixmap>
#include <QWidget>

class ClickableLabel;
class DeviceFinder;
class DeviceHandler;
class DeviceInfoView;
class DeviceNetworkView;
class DevicePairView;
class QBluetoothDeviceInfo;
class QLabel;
class QPushButton;
class Spinner;

class DeviceView : public QWidget
{
    Q_OBJECT

public:
    DeviceView(QWidget* parent = nullptr);
    virtual ~DeviceView();

private:
    void setup_layout();
    void search_for_device();
    void set_edit_mode(bool);

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
