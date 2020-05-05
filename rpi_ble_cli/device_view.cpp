/*
** device_view.cpp
*/

#include "banner_view.h"
#include "clickable_label.h"
#include "device_finder.h"
#include "device_handler.h"
#include "device_info_view.h"
#include "device_network_view.h"
#include "device_pair_view.h"
#include "device_view.h"
#include "spinner.h"
#include "utils.hpp"

#include <QBluetoothDeviceInfo>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

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

    connect(m_device_info, &DeviceInfoView::load_device, [this]() {
        search_for_device();
    });

    connect(m_device_info, &DeviceInfoView::edit_device_info, [this](bool toggle) {
        set_edit_mode(toggle);
    });

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
    m_spinner->setVisible(true);
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
    m_spinner->stop();
    m_spinner->setVisible(false);
    layout()->removeWidget(m_spinner);

    // set device info
    auto name = m_handler->read_device_name();
    auto mac = m_handler->read_device_mac();
    m_device_info->set_device_info(std::move(name), std::move(mac));

    // set device network info
    auto ssid = m_handler->read_network_ssid();
    auto password = m_handler->read_network_password();
    m_device_network->set_network_ssid(std::move(ssid));
    m_device_network->set_network_password(std::move(password));

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
    m_spinner->stop();
    m_spinner->setVisible(false);
    layout()->removeWidget(m_spinner);
}

void DeviceView::set_edit_mode(bool toggle)
{
    m_device_info->set_edit_mode(toggle);
    m_device_network->set_edit_mode(toggle);
}
