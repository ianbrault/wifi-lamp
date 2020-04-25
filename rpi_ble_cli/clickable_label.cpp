/*
** clickable_label.cpp
*/

#include "clickable_label.h"

#include <QMouseEvent>

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
