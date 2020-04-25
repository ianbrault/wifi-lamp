#ifndef BANNER_VIEW_H
#define BANNER_VIEW_H

#include <QPen>
#include <QWidget>

class QPaintEvent;

class BannerView : public QWidget
{
    Q_OBJECT

public:
    BannerView(QWidget* parent = nullptr);
    virtual ~BannerView();
};

class BannerLine : public QWidget
{
    Q_OBJECT

public:
    BannerLine(int width, QWidget* parent = nullptr);
    virtual ~BannerLine();

private:
    virtual void paintEvent(QPaintEvent*);

private:
    QPen m_pen;
};

#endif // BANNER_VIEW_H

