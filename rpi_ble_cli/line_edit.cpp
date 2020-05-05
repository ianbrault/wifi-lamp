/*
** line_edit.cpp
*/

#include "line_edit.h"
#include "utils.hpp"

LineEdit::LineEdit(QWidget* parent)
    : QLineEdit(parent)
{
    m_default_style = QString(
        "border: none; border-bottom: 1px solid black; padding-left: 8px;"
        "background-color: %1"
    ).arg(system_gray_5);

    setStyleSheet(m_default_style);
}

LineEdit::LineEdit(int font_size, QWidget* parent)
    : QLineEdit(parent)
{
    m_default_style = QString(
        "border: none; border-bottom: 1px solid black; padding-left: 4px;"
        "background-color: %1"
    ).arg(system_gray_5);

    set_font_size(font_size);
}

LineEdit::~LineEdit() {}

void LineEdit::set_font_size(int font_size)
{
    setStyleSheet(
        QString("font-size: %1px; %2").arg(font_size).arg(m_default_style));
}
