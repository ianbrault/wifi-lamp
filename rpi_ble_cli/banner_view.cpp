/*
** banner_view.cpp
*/

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

#include "banner_view.h"

static const QColor color(0x49, 0x59, 0x9a);

BannerView::BannerView(QWidget* parent)
    : QWidget(parent)
{
    auto width = this->size().width();

    auto layout = new QHBoxLayout();

    auto image = new QLabel();
    auto image_width = width / 8;

    QPixmap image_icon(":/rpi_bare.svg");
    image_icon.setDevicePixelRatio(3.0);

    image->setPixmap(
        image_icon.scaledToWidth(image_width * 3.0, Qt::SmoothTransformation));
    image->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    auto line_width = (width - image_width) / 2;

    layout->addWidget(new BannerLine(line_width));
    layout->addWidget(image);
    layout->addWidget(new BannerLine(line_width));

    setLayout(layout);
}

BannerView::~BannerView()
{

}

BannerLine::BannerLine(int width, QWidget* parent)
    : QWidget(parent)
{
    m_pen.setColor(color);
    m_pen.setStyle(Qt::SolidLine);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setWidth(4);

    resize(width, 8);
}

BannerLine::~BannerLine()
{

}

void BannerLine::paintEvent(QPaintEvent* event)
{
    if (event) {}  // suppress unused warning

    QPainter painter(this);
    painter.setPen(m_pen);

    auto line_y = (this->size().height() / 2) + 6;
    QPoint start(2, line_y);
    QPoint end(this->size().width() - 2, line_y);
    painter.drawLine(start, end);
}
