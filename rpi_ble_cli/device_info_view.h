#ifndef DEVICE_INFO_VIEW_H
#define DEVICE_INFO_VIEW_H

#include <QPixmap>
#include <QWidget>

#define UNCONNECTED 0
#define CONNECTED   1
#define EDITING     2

class ClickableLabel;
class LineEdit;
class QLabel;
class QVBoxLayout;

class DeviceInfoView : public QWidget
{
    Q_OBJECT

public:
    DeviceInfoView(QWidget* parent = nullptr);
    virtual ~DeviceInfoView();

public:
    void set_device_info(std::string&&, std::string&&);
    void set_edit_mode(bool);

private:
    void set_name(std::string&&);
    void set_mac(std::string&&);

signals:
    void load_device();
    void edit_device_info(bool);
    void update_device_name(QString);

private:
    int m_state = UNCONNECTED;

    QVBoxLayout* m_text_layout;

    QPixmap m_connected;
    QPixmap m_disconnected;
    QPixmap m_load;
    QPixmap m_edit;
    QPixmap m_edit_done;

    QLabel* m_connection_icon;
    QLabel* m_device_name;
    QLabel* m_device_mac;

    LineEdit* m_device_name_edit;

    ClickableLabel* m_button;
};


#endif // DEVICE_INFO_VIEW_H
