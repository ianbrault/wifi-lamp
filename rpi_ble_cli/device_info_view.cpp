/*
** device_info_view.cpp
*/

#include "clickable_label.h"
#include "device_info_view.h"
#include "line_edit.h"
#include "utils.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>

DeviceInfoView::DeviceInfoView(QWidget* parent)
    : QWidget(parent)
{
    m_connected = get_pixmap(":/connected.svg", 45);
    m_disconnected = get_pixmap(":/disconnected.svg", 45);

    m_load = get_pixmap(":/load.svg", 40);
    m_edit = get_pixmap(":/edit.svg", 40);
    m_edit_done = get_pixmap(":/edit_done.svg", 40);

    /*
    ** initialize layout
    */

    auto layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_connection_icon = new QLabel();
    m_connection_icon->setPixmap(m_disconnected);

    m_text_layout = new QVBoxLayout();
    m_text_layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_text_layout->setSpacing(4);

    m_device_name = new QLabel("Not Connected");
    m_device_name->setStyleSheet("font-size: 24px; font-weight: bold");

    m_device_mac = new QLabel("00:00:00:00:00:00");
    m_device_mac->setStyleSheet("font-size: 17px; color: " + system_gray);

    m_text_layout->addWidget(m_device_name);
    m_text_layout->addWidget(m_device_mac);

    m_button = new ClickableLabel();
    m_button->setAlignment(Qt::AlignCenter);
    m_button->setPixmap(m_load);
    connect(m_button, &ClickableLabel::clicked, [this]() {
        if (m_state == UNCONNECTED)
            emit load_device();
        else if (m_state == CONNECTED)
            emit edit_device_info(true);
        else if (m_state == EDITING)
            emit edit_device_info(false);
    });

    m_device_name_edit = new LineEdit(22);

    layout->addSpacing(4);
    layout->addWidget(m_connection_icon);
    layout->addSpacing(16);
    layout->addLayout(m_text_layout);
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    layout->addWidget(m_button);
    layout->addSpacing(4);

    setLayout(layout);
}

DeviceInfoView::~DeviceInfoView() {}

void DeviceInfoView::set_device_info(std::string&& name, std::string&& mac)
{
    m_state = CONNECTED;

    m_connection_icon->setPixmap(m_connected);
    m_button->setPixmap(m_edit);
    set_name(std::move(name));
    set_mac(std::move(mac));
}

void DeviceInfoView::set_edit_mode(bool toggle)
{
    m_state = toggle ? EDITING : CONNECTED;

    if (toggle)
    {
        m_text_layout->replaceWidget(m_device_name, m_device_name_edit);
        m_device_name->setVisible(false);
        m_device_name_edit->setText(m_device_name->text());
        m_device_name_edit->setVisible(true);

        m_button->setPixmap(m_edit_done);
    }
    else
    {
        auto new_name = m_device_name_edit->text();
        // TODO: write updated text to characteristic

        m_text_layout->replaceWidget(m_device_name_edit, m_device_name);
        m_device_name_edit->setVisible(false);
        m_device_name->setText(new_name);
        m_device_name->setVisible(true);

        m_button->setPixmap(m_edit);
    }
}

void DeviceInfoView::set_name(std::string&& name)
{
    m_device_name->setText(QString(name.c_str()));
}

void DeviceInfoView::set_mac(std::string&& mac)
{
    m_device_mac->setText(QString(mac.c_str()));
}
