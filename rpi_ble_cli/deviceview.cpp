/*
** deviceview.cpp
*/

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "devicefinder.h"
#include "devicehandler.h"
#include "deviceview.h"
#include "topbanner.h"

static const QString system_gray("#8e8e93");

DeviceView::DeviceView(QWidget *parent)
    : QWidget(parent)
{
    m_handler = new DeviceHandler(this);
    m_finder  = new DeviceFinder(m_handler, this);

    setup_layout();

    m_finder->start_search();
}

DeviceView::~DeviceView()
{
    delete m_finder;
    m_finder = nullptr;

    delete m_handler;
    m_handler = nullptr;
}

void DeviceView::setup_layout()
{
    auto layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(24, 16, 24, 16);

    m_connected = QPixmap(":/connected.svg")
        .scaledToHeight(45, Qt::SmoothTransformation);
    m_disconnected = QPixmap(":/disconnected.svg")
        .scaledToHeight(45, Qt::SmoothTransformation);

    /*
    ** device information row
    */

    auto device_info_row = new QHBoxLayout();
    device_info_row->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_connection_icon = new QLabel();
    m_connection_icon->setPixmap(m_disconnected);

    auto text_col = new QVBoxLayout();
    text_col->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_device_name = new QLabel("Not Connected");
    m_device_name->setStyleSheet("font-size: 24px; font-weight: bold");

    m_device_mac = new QLabel("00:00:00:00:00:00");
    m_device_mac->setStyleSheet(
        QString("font-size: 17px; color: %1").arg(system_gray));

    text_col->addWidget(m_device_name);
    text_col->addWidget(m_device_mac);

    auto connect_button = new QPushButton(QIcon(":/load.svg"), QString());
    connect_button->setIconSize(QSize(40, 40));
    connect_button->setStyleSheet("background-color: #fff; border-style: none");

    device_info_row->addSpacing(12);
    device_info_row->addWidget(m_connection_icon);
    device_info_row->addSpacing(20);
    device_info_row->addLayout(text_col);
    device_info_row->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    device_info_row->addWidget(connect_button);
    device_info_row->addSpacing(12);

    /*
    ** device control row
    */

    /*
    ** device pair row
    */

    /*
    ** device network row
    */

    layout->addWidget(new TopBanner());
    layout->addSpacing(16);
    layout->addLayout(device_info_row);
    setLayout(layout);
}
