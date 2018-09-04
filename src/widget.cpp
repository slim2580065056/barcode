#include "widget.h"
#include <QPainter>
#include <QPen>
#include <QDebug>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    pos = 0;

    setWindowTitle("条形码生成调试工具");
    this->resize(800, 600);
    b1.setParent(this);
    b1.setText("刷新");
    b1.resize(100,40);
    b1.move(50,520);

    b2 = new QPushButton(this);
    b2->setText("退出");
    b2->resize(100,40);
    b2->move(650,520);

    connect(&b1, &QPushButton::clicked, this, &on_pushButton_clicked);
    connect(b2, &QPushButton::released, this, &Widget::close);
}

Widget::~Widget()
{

}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p; //创建画家
    p.begin(this);
    QPen pen;   //创建画笔
    pen.setWidth(5);
    pen.setColor(Qt::red);
    p.setPen(pen);    //把画笔交给画家
    for(char i=0;i<20;i++)
        p.drawLine(pos,12*i+50,500,500);
    p.end();
}

void Widget::on_pushButton_clicked()
{
    pos += 50;
    if(pos == 800)
        pos = 0;
    qDebug() <<"update: " << pos;
    update();
}
