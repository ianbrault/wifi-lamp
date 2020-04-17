/*
** topbanner.cpp
*/

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

#include "topbanner.h"

static const QColor color(0x49, 0x59, 0x9a);

TopBanner::TopBanner(QWidget* parent)
    : QWidget(parent)
{
    auto width = this->size().width();

    auto layout = new QHBoxLayout();

    auto image = new QLabel();
    auto image_width = width / 8;

    image->setPixmap(
        QPixmap(":/rpi_bare.svg")
            .scaledToWidth(image_width, Qt::SmoothTransformation));
    image->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    auto line_width = (width - image_width) / 2;

    layout->addWidget(new TopBannerLine(line_width));
    layout->addWidget(image);
    layout->addWidget(new TopBannerLine(line_width));

    setLayout(layout);
}

TopBanner::~TopBanner()
{
    
}

TopBannerLine::TopBannerLine(int width, QWidget* parent)
    : QWidget(parent)
{
    m_pen.setColor(color);
    m_pen.setStyle(Qt::SolidLine);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setWidth(4);

    resize(width, 8);
}

TopBannerLine::~TopBannerLine()
{

}

void TopBannerLine::paintEvent(QPaintEvent* event)
{
    if (event) {}  // suppress unused warning

    QPainter painter(this);
    painter.setPen(m_pen);

    auto line_y = (this->size().height() / 2) + 6;
    QPoint start(2, line_y);
    QPoint end(this->size().width() - 2, line_y);
    painter.drawLine(start, end);
}
