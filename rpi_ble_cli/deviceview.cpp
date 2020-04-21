/*
** deviceview.cpp
*/

#include <QDebug>

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "devicefinder.h"
#include "devicehandler.h"
#include "deviceview.h"
#include "topbanner.h"

static const QString system_gray("#8e8e93");

static QPixmap get_pixmap(const QString& file, int height)
{
    auto pxm = QPixmap(file);
    pxm.setDevicePixelRatio(3.0);
    return pxm.scaledToHeight(height * 3.0, Qt::SmoothTransformation);
}

/*
** ClickableLabel
*/

ClickableLabel::ClickableLabel(QWidget* parent)
    : QLabel(parent)
{

}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    if (event) {}
    emit clicked();
}

/*
** DeviceInfoView
*/

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
    // m_connection_icon->setPixmap(m_disconnected);
    m_connection_icon->setPixmap(m_connected);

    auto info_text = new QVBoxLayout();
    info_text->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // m_device_name = new QLabel("Not Connected");
    m_device_name = new QLabel("Arni's Lamp");
    m_device_name->setStyleSheet("font-size: 24px; font-weight: bold");

    // m_device_mac = new QLabel("00:00:00:00:00:00");
    m_device_mac = new QLabel("b8:27:eb:3c:65:98");
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

/*
** DevicePairView
*/

DevicePairView::DevicePairView(QWidget* parent)
    : QWidget(parent)
{
    m_pair_unpaired = get_pixmap(":/pair_unpaired.svg", 45);
    m_pair_connected = get_pixmap(":/pair_connected.svg", 45);
    m_pair_disconnected = get_pixmap(":/pair_disconnected.svg", 45);

    /*
    ** initialize layout
    */

    auto layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_pair_icon = new QLabel();
    // m_pair_icon->setPixmap(m_pair_unpaired);
    m_pair_icon->setPixmap(m_pair_connected);

    // m_pair_text = new QLabel("N/A");
    m_pair_text = new QLabel("Ian's Lamp");
    m_pair_text->setStyleSheet("font-size: 24px");

    layout->addWidget(m_pair_icon);
    layout->addSpacing(12);
    layout->addWidget(m_pair_text);

    setLayout(layout);
}

DevicePairView::~DevicePairView() {}

/*
** DeviceNetworkView
*/

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

    // m_network_name = new QLabel("<b>Network:</b>");
    m_network_name = new QLabel("<b>Network:</b> Tiny Royale");
    m_network_name->setStyleSheet("font-size: 20px");

    // m_network_password = new QLabel("<b>Password:</b> higgins3");
    m_network_password = new QLabel(
        QString("<b>Password:</b> ") +
        QString::fromUtf8("\u2022\u2022\u2022\u2022\u2022\u2022\u2022\u2022"));
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

/*
** DeviceView
*/

DeviceView::DeviceView(QWidget *parent)
    : QWidget(parent)
{
    m_handler = new DeviceHandler(this);
    m_finder  = new DeviceFinder(m_handler, this);

    setup_layout();

    // start searching for the lamp as soon as the view is initialized
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
    layout->setContentsMargins(16, 32, 16, 8);

    m_lamp_on = get_pixmap(":/lamp_on.svg", 80);
    m_lamp_off = get_pixmap(":/lamp_off.svg", 80);

    m_device_on_off = new ClickableLabel();
    m_device_on_off->setAlignment(Qt::AlignCenter);
    m_device_on_off->setPixmap(m_lamp_off);

    connect(m_device_on_off, &ClickableLabel::clicked, this, [this]() {
        m_lamp_is_on = !m_lamp_is_on;
        m_device_on_off->setPixmap(m_lamp_is_on ? m_lamp_on : m_lamp_off);
    });

    m_device_info    = new DeviceInfoView();
    m_device_pair    = new DevicePairView();
    m_device_network = new DeviceNetworkView();

    layout->addWidget(new TopBanner());
    layout->addSpacing(16);
    layout->addWidget(m_device_info);
    layout->addSpacing(50);
    layout->addWidget(m_device_on_off);
    layout->addSpacing(50);
    layout->addWidget(m_device_pair);
    layout->addSpacing(8);
    layout->addWidget(m_device_network);

    setLayout(layout);
}
