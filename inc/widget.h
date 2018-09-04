#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include "code128.h"

class Widget : public QWidget
{
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent*);
public:
    Widget(QWidget *parent = 0);
    ~Widget();
    QPushButton b1;
    QPushButton *b2;
    int  pos;

private slots:
    void on_pushButton_clicked();

};

#endif // WIDGET_H
