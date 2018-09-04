#ifndef CODE128_H
#define CODE128_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>

#define CODE_LEN_MAX    44
#define STOP_CODE       0x8E

class code128 : public QWidget
{
public:
    code128(QWidget* parent = 0);
    ~code128();
    char opetate_code( quint8 *, qint8);
    void draw_code(quint8);

protected:
    void paintEvent(QPaintEvent*);

private:
    QPushButton *ba;    //draw bar code a
    QPushButton *bb;    //draw bar code b
    QPushButton *bc;    //draw bar code c
    QPushButton b3;     //close

    QLineEdit   *txt;   //lineEdit
    QLabel      *label; //label

    QLabel      *label_LW;  //set line width
    QLabel      *label_LH;  //set line hight
    QLineEdit   *txt_LW;  //set line width
    QLineEdit   *txt_LH;  //set line hight

    QRadioButton *rb1;  //radio button

    QLabel      *label_forecolor;   //forecolor
    QLabel      *label_backcolor;   //backcolor
    QLineEdit   *txt_fcr;
    QLineEdit   *txt_fcg;
    QLineEdit   *txt_fcb;
    QLineEdit   *txt_bcr;
    QLineEdit   *txt_bcg;
    QLineEdit   *txt_bcb;


    quint8 str_len;    //字符长度（不计入起始+选码+校验+结束+操作码）
    quint8 cnt;
    quint16 tmp_code[CODE_LEN_MAX]; //所有字符组成的字节流数量
    quint8 line_W;      //条形码线宽
    quint16 line_H;     //条形码线高
    quint8  div_line;   //调试模型下的分割线间隙
    quint8  fcr;
    quint8  fcg;
    quint8  fcb;
    quint8  bcr;
    quint8  bcg;
    quint8  bcb;
    quint8  flag_c;     //code C 编码要特殊处理一下
    quint8  line_txt[CODE_LEN_MAX];
    quint8  line_txt_len;
    quint8  line_txt_isnum; //判断lineedit是否全为数字

private slots:
    void draw_codeA();
    void draw_codeB();
    void draw_codeC();
    void make_hexarr(int);
    quint8 calc_tail(int);
};

#endif // CODE128_H
