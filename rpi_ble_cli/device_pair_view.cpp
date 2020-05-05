/*
** device_pair_view.cpp
*/

#include "device_pair_view.h"
#include "utils.hpp"

#include <QHBoxLayout>
#include <QLabel>

DevicePairView::DevicePairView(QWidget* parent)
    : QWidget(parent)
{
    m_pair_unpaired = get_pixmap(":/pair_unpaired.svg", 40);
    m_pair_connected = get_pixmap(":/pair_connected.svg", 40);
    m_pair_disconnected = get_pixmap(":/pair_disconnected.svg", 40);

    /*
    ** initialize layout
    */

    auto layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_pair_icon = new QLabel();
    m_pair_icon->setPixmap(m_pair_unpaired);

    m_pair_text = new QLabel("N/A");
    m_pair_text->setStyleSheet("font-size: 22px");

    layout->addWidget(m_pair_icon);
    layout->addSpacing(12);
    layout->addWidget(m_pair_text);

    setLayout(layout);
}

DevicePairView::~DevicePairView() {}
