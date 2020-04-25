#ifndef CLICKABLE_LABEL_H
#define CLICKABLE_LABEL_H

#include <QLabel>

class QMouseEvent;

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget* parent = nullptr);
    virtual ~ClickableLabel();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent*);
};

#endif // CLICKABLE_LABEL_H
