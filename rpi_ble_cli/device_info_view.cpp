/*
** device_info_view.cpp
*/

#include "clickable_label.h"
#include "device_info_view.h"
#include "utils.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>

DeviceInfoView::DeviceInfoView(QWidget* parent)
    : QWidget(parent)
{
    m_connected = get_pixmap(":/connected.svg", 45);
    m_disconnected = get_pixmap(":/disconnected.svg", 45);

    /*
    ** initialize layout
    */

    auto layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_connection_icon = new QLabel();
    m_connection_icon->setPixmap(m_disconnected);

    auto info_text = new QVBoxLayout();
    info_text->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    info_text->setSpacing(4);

    m_device_name = new QLabel("Not Connected");
    m_device_name->setStyleSheet("font-size: 24px; font-weight: bold");

    m_device_mac = new QLabel("00:00:00:00:00:00");
    m_device_mac->setStyleSheet("font-size: 17px; color: " + system_gray);

    info_text->addWidget(m_device_name);
    info_text->addWidget(m_device_mac);

    auto connect_button = new ClickableLabel();
    connect_button->setAlignment(Qt::AlignCenter);
    connect_button->setPixmap(get_pixmap(":/load.svg", 36));

    layout->addSpacing(4);
    layout->addWidget(m_connection_icon);
    layout->addSpacing(16);
    layout->addLayout(info_text);
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    layout->addWidget(connect_button);
    layout->addSpacing(4);

    setLayout(layout);
}

DeviceInfoView::~DeviceInfoView() {}

void DeviceInfoView::set_device_info(std::string&& name, std::string&& mac)
{
    m_connection_icon->setPixmap(m_connected);
    set_name(std::move(name));
    set_mac(std::move(mac));
}

void DeviceInfoView::set_name(std::string&& name)
{
    m_device_name->setText(QString(name.c_str()));
}

void DeviceInfoView::set_mac(std::string&& mac)
{
    m_device_mac->setText(QString(mac.c_str()));
}
