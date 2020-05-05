#ifndef LINE_EDIT_H
#define LINE_EDIT_H

#include <QLineEdit>

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(QWidget* parent = nullptr);
    LineEdit(int font_size, QWidget* parent = nullptr);
    virtual ~LineEdit();

public:
    void set_font_size(int);

private:
    QString m_default_style;
};

#endif

