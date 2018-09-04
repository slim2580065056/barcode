#ifndef QRCODE_H
#define QRCODE_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QComboBox>

class QRcode : public QWidget
{
public:
    QRcode(QWidget* parents = 0);
    ~QRcode();

private:
    QPainter    pqr; //创建画家
    QPen        pen; //创建画笔
    QPushButton *bc;
    QLabel      *ver_label;
    QLabel      *mask_label;
    QLabel      *pixel_label;
    QLabel      *AEC_level_label; //纠错等级
    QLabel      *BCH_label; //BCH位
    QLabel      *disp_label;    //展示调试标签
    QLabel      *encode_label;  //编码规则
    QLineEdit   *ver_txt;
    QLineEdit   *mask_txt;
    QLineEdit   *pixel_txt;
    QComboBox   *AEC_level;
    QComboBox   *encoding;
    QLineEdit   *BCH_txt;   //BCH

    void draw_base(quint8 ver);
    void draw_mask(quint8 mask);
    void draw_info(quint16 info);

    //quint8 line_W;
    quint8 ver_W;
    quint8 pixel;       //二维码像素点
    quint8 div_line;    //分割线用于调试
    quint16 format_info;    //格式信息
    quint16 ver_info;       //版本信息，v7-v40才有
    quint16 offset_W = 120;   //水平偏移
    quint16 offset_H = 120;   //垂直偏移

private slots:
    void create_qrcode(void);

protected:
    void paintEvent(QPaintEvent*);
};



#endif // QRCODE_H
