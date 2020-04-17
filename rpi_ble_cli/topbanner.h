#ifndef TOPBANNER_H
#define TOPBANNER_H

#include <QPen>
#include <QWidget>

class QPaintEvent;

class TopBanner : public QWidget
{
    Q_OBJECT

public:
    TopBanner(QWidget* parent = nullptr);
    virtual ~TopBanner();
};

class TopBannerLine : public QWidget
{
    Q_OBJECT

public:
    TopBannerLine(int width, QWidget* parent = nullptr);
    virtual ~TopBannerLine();

private:
    virtual void paintEvent(QPaintEvent*);

private:
    QPen m_pen;
};

#endif // TOPBANNER_H

