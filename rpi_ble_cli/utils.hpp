#ifndef UTILS_HPP
#define UTILS_HPP

#include <QPixmap>
#include <QString>

const QString system_gray("#8e8e93");

QPixmap get_pixmap(const QString& file, int height)
{
    auto pxm = QPixmap(file);
    pxm.setDevicePixelRatio(3.0);
    return pxm.scaledToHeight(height * 3.0, Qt::SmoothTransformation);
}

#endif // UTILS_HPP
