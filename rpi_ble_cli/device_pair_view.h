#ifndef DEVICE_PAIR_VIEW_H
#define DEVICE_PAIR_VIEW_H

#include <QPixmap>
#include <QWidget>

class QLabel;

class DevicePairView : public QWidget
{
    Q_OBJECT

public:
    DevicePairView(QWidget* parent = nullptr);
    virtual ~DevicePairView();

private:
    QPixmap m_pair_unpaired;
    QPixmap m_pair_connected;
    QPixmap m_pair_disconnected;

    QLabel* m_pair_icon;
    QLabel* m_pair_text;
};

#endif // DEVICE_PAIR_VIEW_H

