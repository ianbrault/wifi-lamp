/*
** device_view.cpp
*/

#include "banner_view.h"
#include "clickable_label.h"
#include "device_finder.h"
#include "device_handler.h"
#include "device_info_view.h"
#include "device_network_view.h"
#include "device_view.h"
#include "spinner.h"
#include "utils.hpp"

#include <QBluetoothDeviceInfo>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

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
    m_pair_icon->setPixmap(m_pair_unpaired);

    m_pair_text = new QLabel("N/A");
    m_pair_text->setStyleSheet("font-size: 24px");

    layout->addWidget(m_pair_icon);
    layout->addSpacing(12);
    layout->addWidget(m_pair_text);

    setLayout(layout);
}

DevicePairView::~DevicePairView() {}

/*
** DeviceView
*/

DeviceView::DeviceView(QWidget *parent)
    : QWidget(parent)
{
    m_handler = new DeviceHandler(this);
    m_finder  = new DeviceFinder(m_handler, this);

    connect(
        m_finder, &DeviceFinder::device_not_found,
        this,     &DeviceView::device_not_found);

    connect(
        m_handler, &DeviceHandler::device_found,
        this,      &DeviceView::device_found);
    // TODO: need a better way to handle errors here
    connect(
        m_handler, &DeviceHandler::device_characteristic_error,
        this,      &DeviceView::device_not_found);

    setup_layout();

    // start searching for the lamp as soon as the view is initialized
    search_for_device();
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

    // initialize pixmaps
    m_lamp_on = get_pixmap(":/lamp_on.svg", 80);
    m_lamp_off = get_pixmap(":/lamp_off.svg", 80);

    // initialize spinner
    m_spinner = new Spinner(nullptr, false, false);
    m_spinner->setRoundness(72.0);
    m_spinner->setMinimumTrailOpacity(16.0);
    m_spinner->setNumberOfLines(12);
    m_spinner->setLineLength(14);
    m_spinner->setLineWidth(4);
    m_spinner->setRevolutionsPerSecond(1.6);

    // initialize device child views
    m_device_info    = new DeviceInfoView();
    m_device_pair    = new DevicePairView();
    m_device_network = new DeviceNetworkView();

    // initialize device control view
    m_device_on_off = new ClickableLabel();
    m_device_on_off->setAlignment(Qt::AlignCenter);
    m_device_on_off->setPixmap(m_lamp_off);

    connect(m_device_on_off, &ClickableLabel::clicked, this, [this]() {
        m_lamp_is_on = !m_lamp_is_on;
        m_device_on_off->setPixmap(m_lamp_is_on ? m_lamp_on : m_lamp_off);
    });

    layout->addWidget(new BannerView());
    layout->addSpacing(16);
    layout->addWidget(m_device_info);
    layout->addSpacing(50);

    setLayout(layout);
}

void DeviceView::search_for_device()
{
    if (m_searching || m_device_found)
        return;

    m_searching = true;

    // show the spinner
    m_spinner->start();
    layout()->addWidget(m_spinner);
    layout()->itemAt(layout()->count() - 1)->setAlignment(Qt::AlignCenter);

    // start DeviceFinder search
    m_finder->start_search();
}

void DeviceView::device_found()
{
    m_searching = false;
    m_device_found = true;

    // remove the spinner
    layout()->removeWidget(m_spinner);

    // set device info
    auto name = m_handler->read_device_name();
    auto mac = m_handler->read_device_mac();
    m_device_info->set_device_info(std::move(name), std::move(mac));

    // add device views
    layout()->addWidget(m_device_on_off);
    ((QBoxLayout*) layout())->addSpacing(50);
    layout()->addWidget(m_device_pair);
    ((QBoxLayout*) layout())->addSpacing(8);
    layout()->addWidget(m_device_network);
}

void DeviceView::device_not_found()
{
    m_searching = false;
    m_device_found = false;

    // remove the spinner
    layout()->removeWidget(m_spinner);
}
