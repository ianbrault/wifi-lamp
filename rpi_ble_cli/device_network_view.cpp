/*
** device_network_view.cpp
*/

#include "device_network_view.h"
#include "line_edit.h"
#include "utils.hpp"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

DeviceNetworkView::DeviceNetworkView(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto network_icon = new QLabel();
    network_icon->setPixmap(get_pixmap(":/globe.svg", 40));

    m_text_layout = new QGridLayout();
    m_text_layout->setHorizontalSpacing(16);

    auto ssid_label = new QLabel("<b>Network:</b>");
    ssid_label->setStyleSheet("font-size: 18px");

    m_network_ssid = new QLabel();
    m_network_ssid->setStyleSheet("font-size: 18px");

    auto password_label = new QLabel("<b>Password:</b>");
    password_label->setStyleSheet("font-size: 18px");

    m_network_password = new QLabel();
    m_network_password->setStyleSheet("font-size: 18px");

    m_text_layout->addWidget(ssid_label, 0, 0);
    m_text_layout->addWidget(password_label, 1, 0);
    m_text_layout->addWidget(m_network_ssid, 0, 1);
    m_text_layout->addWidget(m_network_password, 1, 1);

    m_network_ssid_edit = new LineEdit(18);
    m_network_password_edit = new LineEdit(18);

    layout->addSpacing(9);
    layout->addWidget(network_icon);
    layout->addSpacing(12);
    layout->addLayout(m_text_layout);

    setLayout(layout);
}

DeviceNetworkView::~DeviceNetworkView() {}

void DeviceNetworkView::set_network_ssid(std::string&& ssid)
{
    m_network_ssid->setText(ssid.c_str());
}

void DeviceNetworkView::set_network_password(std::string&& password)
{
    // password dot is UTF-8: \u2022
    QString redacted;
    for (size_t i = 0; i < password.length(); i++)
        redacted.append("\u2022");

    m_network_password->setText(redacted);
}

void DeviceNetworkView::set_edit_mode(bool toggle)
{
    if (toggle)
    {
        m_text_layout->replaceWidget(m_network_ssid, m_network_ssid_edit);
        m_text_layout->replaceWidget(m_network_password, m_network_password_edit);

        m_network_ssid->setVisible(false);
        m_network_password->setVisible(false);

        m_network_ssid_edit->setText(m_network_ssid->text());
        m_network_password_edit->setText(m_network_password->text());

        m_network_ssid_edit->setVisible(true);
        m_network_password_edit->setVisible(true);
    }
    else
    {
        auto new_ssid = m_network_ssid_edit->text();
        auto new_password = m_network_password_edit->text();
        // TODO: write updated text to characteristics

        m_text_layout->replaceWidget(m_network_ssid_edit, m_network_ssid);

        m_network_ssid_edit->setVisible(false);
        m_network_password_edit->setVisible(false);

        m_network_ssid->setText(new_ssid);
        m_network_password->setText(new_password);

        m_network_ssid->setVisible(true);
        m_network_password->setVisible(true);
    }
}
