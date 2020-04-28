/*
** device_network_view.cpp
*/

#include "device_network_view.h"
#include "utils.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

DeviceNetworkView::DeviceNetworkView(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto network_icon = new QLabel();
    network_icon->setPixmap(get_pixmap(":/globe.svg", 45));

    auto network_text = new QVBoxLayout();
    network_text->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    network_text->setSpacing(2);

    m_network_name = new QLabel("<b>Network:</b>");
    m_network_name->setStyleSheet("font-size: 20px");

    // password dot is UTF-8: \u2022
    m_network_password = new QLabel("<b>Password:</b>");
    m_network_password->setStyleSheet("font-size: 20px");

    network_text->addWidget(m_network_name);
    network_text->addWidget(m_network_password);

    layout->addSpacing(9);
    layout->addWidget(network_icon);
    layout->addSpacing(12);
    layout->addLayout(network_text);

    setLayout(layout);
}

DeviceNetworkView::~DeviceNetworkView() {}
