#ifndef UTILS_HPP
#define UTILS_HPP

#include <QPixmap>
#include <QString>

const QString system_gray("#8e8e93");
// not actually systemGray5, edited to be lighter
const QString system_gray_5("#eaeaef");

QPixmap get_pixmap(const QString&, int);

#endif // UTILS_HPP
