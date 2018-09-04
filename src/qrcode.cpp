#include "qrcode.h"
#include <qvalidator.h>
#include <QDebug>

#define mask_xor  0x5412  //101010000010010掩码图形，小型码用不到

quint8 base_code[]={    //基础定位图形
    0xFE, 0x82, 0xBA, 0xBA,
    0xBA, 0x82, 0xFE, 0x00,
};

QRcode::QRcode(QWidget *parent):QWidget(parent)
{
    format_info = 0;
    setWindowTitle("二维码生成工具");
    this->resize(800,600);

    bc = new QPushButton(this);
    bc->setStyleSheet("QPushButton{font-size:25px;color:red;}");
    bc->setText("生成");
    bc->resize(60,60);
    bc->move(20,20);
    ver_label = new QLabel(this);
    ver_label->move(120, 30);
    ver_label->setText("版本");
    mask_label = new QLabel(this);
    mask_label->move(120,60);
    mask_label->setText("掩码");
    pixel_label = new QLabel(this);
    pixel_label->move(230, 30);
    pixel_label->setText("像素");
    disp_label = new QLabel(this);
    disp_label->move(220, 10);
    disp_label->setText("QQ 2580065056");
    BCH_label = new QLabel(this);
    BCH_label->move(220, 60);
    BCH_label->setText("BCH位");
    AEC_level_label = new QLabel(this);
    AEC_level_label->move(320, 60);
    AEC_level_label->setText("纠错等级");
    AEC_level = new QComboBox(this);
    AEC_level->resize(65,20);
    AEC_level->move(385, 58);
    AEC_level->addItem("L(01)");
    AEC_level->addItem("M(00)");
    AEC_level->addItem("Q(11)");
    AEC_level->addItem("H(10)");
    encode_label = new QLabel(this);
    encode_label->move(320, 30);
    encode_label->setText("编码规则");
    encoding = new QComboBox(this);
    encoding->resize(85,20);
    encoding->move(385, 28);
    encoding->addItem("数字编码1H");
    encoding->addItem("字符编码2H");
    encoding->addItem("字节编码4H");
    encoding->addItem("中文编码DH");
    encoding->addItem("混合编码3H");
    encoding->addItem("其他编码");

    ver_txt = new QLineEdit(this);
    ver_txt->resize(40, 20);
    ver_txt->move(155, 28);
    ver_txt->setValidator(new QIntValidator(1, 40, this));  //设置版本范围为1-40
    ver_txt->setAlignment(Qt::AlignHCenter);    //居中
    ver_txt->setStyleSheet("QLineEdit{font-size:16px;}");
    ver_txt->setText("1");
    mask_txt = new QLineEdit(this);
    mask_txt->resize(40, 20);
    mask_txt->move(155, 58);
    mask_txt->setValidator(new QIntValidator(0, 7, this));  //设置掩码有效范围
    mask_txt->setAlignment(Qt::AlignHCenter);    //居中
    mask_txt->setStyleSheet("QLineEdit{font-size:16px;}");
    mask_txt->setText("5");
    pixel_txt = new QLineEdit(this);
    pixel_txt->resize(30,20);
    pixel_txt->move(265, 28);
    pixel_txt->setValidator(new QIntValidator(2, 30, this));  //设置像素点有效范围
    pixel_txt->setAlignment(Qt::AlignHCenter);    //居中
    pixel_txt->setStyleSheet("QLineEdit{font-size:16px;}");
    pixel_txt->setText("15");
    BCH_txt = new QLineEdit(this);
    BCH_txt->resize(40, 20);
    BCH_txt->move(265, 58);
    BCH_txt->setValidator(new QIntValidator(0, 1023, this));    //设置BCH位有效范围
    BCH_txt->setAlignment(Qt::AlignHCenter);    //居中
    BCH_txt->setStyleSheet("QLineEdit{font-size:16px;}");
    BCH_txt->setText("438");    //0110110110

    connect(bc, &QPushButton::released, this, &create_qrcode);
}

QRcode::~QRcode()
{

}

void QRcode::paintEvent(QPaintEvent*)
{
    pixel = pixel_txt->text().toUShort();
    quint8 ver = ver_txt->text().toShort();
    if(ver < 1) {
        ver_txt->setText("1");
    }
    if(pixel < 2){
        pixel_txt->setText("2");
    }
    quint8 mask = mask_txt->text().toShort();
    ver_W = 7+(4*ver+3);   //7 + (4*ver+3) //偏移到右边和下面的定位符7个格

    pqr.begin(this);
    if(format_info != 0){
        draw_base(ver);
        draw_mask(mask);
        draw_info(format_info);
    }
    pqr.end();
}

void QRcode::create_qrcode(void)
{
    qDebug("create qrcode");
    //求格式信息
    if("L(01)" == AEC_level->currentText()){
        format_info = 0x01<<13 | mask_txt->text().toUShort()<<10 | BCH_txt->text().toUShort();
    }
    else if("M(00)" == AEC_level->currentText()){
        format_info = 0x00<<13 | mask_txt->text().toUShort()<<10 | BCH_txt->text().toUShort();
    }
    else if("Q(11)" == AEC_level->currentText()){
        format_info = 0x03<<13 | mask_txt->text().toUShort()<<10 | BCH_txt->text().toUShort();
    }
    else if("H(10)" == AEC_level->currentText()){
        format_info = 0x02<<13 | mask_txt->text().toUShort()<<10 | BCH_txt->text().toUShort();
    }
    format_info ^= mask_xor;    //模块信息加密后的格式信息模块图形
    qDebug("format: %x",format_info);
    disp_label->setText(AEC_level->currentText());
    update();
}

void QRcode::draw_base(quint8 ver)//绘制基础框图
{
    pen.setWidth(pixel-1);
    pen.setColor(Qt::red);
    pqr.setPen(pen);    //把画笔交给画家
    for(quint8 n=0;n<3;n++){
        for(quint8 i=0;i<8;i++){
            quint8 tmp = base_code[i];
            //qDebug("%d: %x",i,tmp);
            for(quint8 j=0;j<8;j++){
                if(tmp & 0x80){
                    pqr.drawLine(100+pixel*ver_W*(n%2)+pixel*i, 100+pixel*ver_W*(n/2)+pixel*j, 100+pixel*ver_W*(n%2)+pixel*i, 100+pixel*ver_W*(n/2)+pixel*j);
                }
                if(6 == j){
                    for(quint8 n=0;n<(ver_W-9);){  //位置探测图形的空白区偏移量
                        pqr.drawLine(100+pixel*8+pixel*n, 100+pixel*j, 100+pixel*8+pixel*n, 100+pixel*j);
                        n += 2;
                    }
                }
                tmp <<= 1;
            }
            if(6 == i){
                for(quint8 n=0;n<(ver_W-9);){  //位置探测图形的空白区偏移量
                    pqr.drawLine(100+pixel*i, 100+pixel*8+pixel*n, 100+pixel*i, 100+pixel*8+pixel*n);
                    n += 2;
                }
                pqr.drawLine(100+pixel*(i+2), 100+pixel*8+pixel*(ver_W-9), 100+pixel*(i+2), 100+pixel*8+pixel*(ver_W-9)); //Dark Module
            }
        }
    }
}

void QRcode::draw_mask(quint8 mask)//绘制掩码
{
    pen.setWidth(pixel-1);
    pen.setColor(Qt::green);
    pqr.setPen(pen);    //把画笔交给画家
    for(quint8 i=0;i<7+ver_W;i++){
        for(quint8 j=0;j<7+ver_W;j++){
            if((i>8 && i<ver_W-1) ||(j>8 && j<ver_W-1) || (i>ver_W-2 && j>ver_W-2)){    //去除定位区域
                if(mask == 0 && !((i+j)%2) && i!=6 && j!=6){
                    pqr.drawLine(100+pixel*i, 100+pixel*j, 100+pixel*i, 100+pixel*j);
                }
                else if(mask == 1 && !(j%2) && i!=6 && j!=6){
                    pqr.drawLine(100+pixel*i, 100+pixel*j, 100+pixel*i, 100+pixel*j);
                }
                else if(mask == 2 && !(i%3) && i!=6 && j!=6){
                    pqr.drawLine(100+pixel*i, 100+pixel*j, 100+pixel*i, 100+pixel*j);
                }
                else if(mask == 3 && !((i+j)%3) && i!=6 && j!=6){
                    pqr.drawLine(100+pixel*i, 100+pixel*j, 100+pixel*i, 100+pixel*j);
                }
                else if(mask == 4 && !(((i/3)+(j/2))%2) && i!=6 && j!=6){
                    pqr.drawLine(100+pixel*i, 100+pixel*j, 100+pixel*i, 100+pixel*j);
                }
                else if(mask == 5 && !((i*j)%2+(i*j)%3) && i!=6 && j!=6){
                    pqr.drawLine(100+pixel*i, 100+pixel*j, 100+pixel*i, 100+pixel*j);
                }
                else if(mask == 6 && !(((i*j)%2+(i*j)%3)%2) && i!=6 && j!=6){
                    pqr.drawLine(100+pixel*i, 100+pixel*j, 100+pixel*i, 100+pixel*j);
                }
                else if(mask == 7 && !(((i+j)%2+(i*j)%3)%2) && i!=6 && j!=6){
                    pqr.drawLine(100+pixel*i, 100+pixel*j, 100+pixel*i, 100+pixel*j);
                }
            }
        }
    }
}

void QRcode::draw_info(quint16 info)
{
    pen.setWidth(pixel-1);
    pen.setColor(Qt::blue);
    pqr.setPen(pen);    //把画笔交给画家
    quint8 info_H = format_info>>8&0xff;
    quint8 info_L = format_info&0xff;
    //竖直位置
    for(quint8 i=0;i<8;i++){
        if(info_L & 0x01){
            if(i > 5)
                pqr.drawLine(100+pixel*8, 100+pixel*(i+1), 100+pixel*8, 100+pixel*(i+1));   //竖直位置
            else
                pqr.drawLine(100+pixel*8, 100+pixel*i, 100+pixel*8, 100+pixel*i);   //竖直位置
            pqr.drawLine(100+pixel*(ver_W+6-i), 100+pixel*8, 100+pixel*(ver_W+6-i), 100+pixel*8);   //水平位置
        }
        info_L >>= 1;

        if((i < 7) && (info_H & 0x01)){
            pqr.drawLine(100+pixel*8, 100+pixel*i+pixel*ver_W, 100+pixel*8, 100+pixel*i+pixel*ver_W);   //竖直位置
            if(i<1)
                pqr.drawLine(100+pixel*(7-i), 100+pixel*8, 100+pixel*(7-i), 100+pixel*8);   //水平位置
            else
                pqr.drawLine(100+pixel*(6-i), 100+pixel*8, 100+pixel*(6-i), 100+pixel*8);   //水平位置
        }
        info_H >>= 1;
    }
}
