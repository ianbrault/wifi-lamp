/*
** utils.cpp
*/

#include "utils.hpp"

QPixmap get_pixmap(const QString& file, int height)
{
    auto pxm = QPixmap(file);
    pxm.setDevicePixelRatio(3.0);
    return pxm.scaledToHeight(height * 3.0, Qt::SmoothTransformation);
}
