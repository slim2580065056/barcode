#include "code128.h"
#include <QDebug>
#include <QPainter>
#include <QPen>


quint8 msg1[] = {'L','i','u','S','o','n','g','H','u'};
quint8 msg2[] = {'H','e','l','l','o',' ','W','o','r','l','d'};
quint8 msg3[] = {'2','5','8','0','0','6','5','0','5','6'};

const quint8 code_table[] = {
    0xb3,0x9b,0x99,0x26,0x23,0x13,0x32,0x31,0x19,0x92,
    0x91,0x89,0x67,0x37,0x33,0x73,0x3b,0x39,0x9c,0x97,
    0x93,0xb9,0x9d,0xdb,0xd3,0xcb,0xc9,0xd9,0xcd,0xcc,
    0xb6,0xb1,0x8d,0x46,0x16,0x11,0x62,0x1a,0x18,0xa2,
    0x8a,0x88,0x6e,0x63,0x1b,0x76,0x71,0x1d,0xdd,0xa3,
    0x8b,0xba,0xb8,0xbb,0xd6,0xd1,0xc5,0xda,0xd8,0xc6,
    0xde,0x90,0xe2,0x4c,0x43,0x2c,0x21,0x0b,0x09,0x64,
    0x61,0x34,0x30,0x0d,0x0c,0x84,0x94,0xee,0x85,0x1e,
    0x4f,0x2f,0x27,0x79,0x3d,0x3c,0xe9,0xe5,0xe4,0xb7,
    0xbd,0xed,0x5e,0x47,0x17,0x7a,0x78,0xea,0xe8,0x77,
    0x7b,0xd7,0xeb,0xa1,0xa4,0xa7
    //99->code_C / 100->code_B / 101->code_A / 103->start_A / 104->start_B / 105->start_C / 106->stop
};

code128::code128(QWidget *parent):QWidget(parent)
{
    str_len = 0;
    setWindowTitle("条形码绘制窗口");
    this->resize(1000,500);
    ba = new QPushButton(this);
    ba->setText("codeA");
    ba->resize(100,40);
    ba->move(80, height()-80);

    bb = new QPushButton(this);
    bb->setText("codeB");
    bb->resize(100,40);
    bb->move(width()/4+80, height()-80);

    bc = new QPushButton(this);
    bc->setText("codeC");
    bc->resize(100,40);
    bc->move(width()/4*2+80, height()-80);

    b3.setParent(this);
    b3.setText("退出系统");
    b3.resize(100,40);
    b3.move(width()/4*3+80, height()-80);

    connect(ba, &QPushButton::released, this, &draw_codeA);
    connect(bb, &QPushButton::released, this, &draw_codeB);
    connect(bc, &QPushButton::released, this, &draw_codeC);
    connect(&b3, &QPushButton::released, this, &close);

    //中心文本框
    txt = new QLineEdit(this);
    txt->resize(600,80);
    txt->move(200, 50);
    txt->setAlignment(Qt::AlignHCenter);    //居中
    txt->setMaxLength(CODE_LEN_MAX);    //设置输入最大字符串长度
    txt->setStyleSheet("QLineEdit{font-size:30px;font-weight:500;color:red;}");
    //txt->setFont(Qt::SizeMode);

    //中心标签
    label = new QLabel(this);
    label->move(400, 20);
    label->setText("刘松虎 QQ2580065056");

    //设置条形码参数
    label_LW = new QLabel(this);
    label_LW->move(30, 60);
    label_LW->setText("宽度:");
    label_LH = new QLabel(this);
    label_LH->move(30, 90);
    label_LH->setText("高度:");
    txt_LW = new QLineEdit(this);
    txt_LW->resize(40, 20);
    txt_LW->move(80, 60);
    txt_LW->setText("5");
    txt_LH = new QLineEdit(this);
    txt_LH->resize(40, 20);
    txt_LH->move(80, 90);
    txt_LH->setText("200");
    //调试单选按钮
    rb1 = new QRadioButton(this);
    rb1->move(30, 30);
    rb1->setText("调试模式");
    //颜色选项
    label_forecolor = new QLabel(this);
    label_forecolor->move(840,40);
    label_forecolor->setText("前景色");
    txt_fcr = new QLineEdit(this);
    txt_fcr->resize(50,20);
    txt_fcr->move(840,60);
    txt_fcr->setStyleSheet("background-color:red;");
    txt_fcr->setText("0");
    txt_fcg = new QLineEdit(this);
    txt_fcg->resize(50,20);
    txt_fcg->move(840,84);
    txt_fcg->setStyleSheet("background-color:green;");
    txt_fcg->setText("0");
    txt_fcb = new QLineEdit(this);
    txt_fcb->resize(50,20);
    txt_fcb->move(840,108);
    txt_fcb->setStyleSheet("background-color:blue;");
    txt_fcb->setText("0");
    label_backcolor = new QLabel(this);
    label_backcolor->move(920,40);
    label_backcolor->setText("背景色");
    txt_bcr = new QLineEdit(this);
    txt_bcr->resize(50,20);
    txt_bcr->move(920,60);
    txt_bcr->setStyleSheet("background-color:red;");
    txt_bcr->setText("255");
    txt_bcg = new QLineEdit(this);
    txt_bcg->resize(50,20);
    txt_bcg->move(920,84);
    txt_bcg->setStyleSheet("background-color:green;");
    txt_bcg->setText("255");
    txt_bcb = new QLineEdit(this);
    txt_bcb->resize(50,20);
    txt_bcb->move(920,108);
    txt_bcb->setStyleSheet("background-color:blue;");
    txt_bcb->setText("255");
}

code128::~code128()
{

}

void code128::paintEvent(QPaintEvent *)
{
    if(width()<1000 || height()<500){   //设置主窗口最小尺寸
        resize(1100,500);
    }
    if(rb1->isChecked()){   //判断单选按钮选项
        txt_LW->setText("10");
        txt_LW->setReadOnly(true);
        txt_LH->setText("200");
        txt_LW->setReadOnly(true);
        div_line = 2;
    }
    else{
        txt_LW->setReadOnly(false);
        txt_LH->setReadOnly(false);
        div_line = 0;
    }
    line_W = txt_LW->text().toUInt();   //设置及判断条形码宽和高
    if(line_W > 10 || line_W < 2){
        txt_LW->setText("2");
        line_W = 2;
    }
    line_H = txt_LH->text().toUInt();
    if(line_H > 1000 || line_H <10){
        txt_LH->setText("200");
        line_H = 200;
    }
    fcr = txt_fcr->text().toUShort();
    fcg = txt_fcg->text().toUShort();
    fcb = txt_fcb->text().toUShort();
    bcr = txt_bcr->text().toUShort();
    bcg = txt_bcg->text().toUShort();
    bcb = txt_bcb->text().toUShort();

    label_forecolor->setStyleSheet("background-color:qRgb(fcr,fcg,fcb);");
    QPainter pbar; //创建画家
    pbar.begin(this);
    QPen pen;   //创建画笔
    pen.setWidth(5);
    pen.setColor(qRgb(fcr,fcg,fcb));
    pbar.setPen(pen);    //把画笔交给画家
    pbar.drawRect(830,30,75,118);
    pen.setColor(qRgb(bcr,bcg,bcb));
    pbar.setPen(pen);    //把画笔交给画家
    pbar.drawRect(910,30,75,118);
    //绘制条形码
    if(str_len > 0){
        pen.setWidth(line_W-div_line);
        for(quint8 n=0;n<(str_len+3);n++){
            quint16 tmp = tmp_code[n];
            for(quint8 i=0;i<11;i++){
                if(tmp&0x400){
                    pen.setColor(qRgb(fcr,fcg,fcb));
                    pbar.setPen(pen);
                    pbar.drawLine(100+line_W*i+(n*line_W*11), 200, 100+line_W*i+(n*line_W*11), 200+line_H);
                }
                else{
                    if(!flag_c){    //code c 不使用背景色
                        pen.setColor(qRgb(bcr,bcg,bcb));
                        pbar.setPen(pen);
                        pbar.drawLine(100+line_W*i+(n*line_W*11), 200, 100+line_W*i+(n*line_W*11), 200+line_H);
                    }
                }
                tmp <<= 1;
            }
        }
        {   //stop bit more than 2bit
            pen.setColor(qRgb(fcr,fcg,fcb));
            pbar.setPen(pen);
            if(flag_c){  //code c长度要对半分
                pbar.drawLine(100+((str_len/2+(str_len%2)+3)*line_W*11), 200,100+((str_len/2+(str_len%2)+3)*line_W*11), 200+line_H);
                pbar.drawLine(100+line_W+((str_len/2+(str_len%2)+3)*line_W*11), 200,100+line_W+((str_len/2+(str_len%2)+3)*line_W*11), 200+line_H);
            }
            else{
                pbar.drawLine(100+((str_len+3)*line_W*11), 200,100+((str_len+3)*line_W*11), 200+line_H);
                pbar.drawLine(100+line_W+((str_len+3)*line_W*11), 200,100+line_W+((str_len+3)*line_W*11), 200+line_H);
            }
        }
    }

    pen.setColor(Qt::green);
    pen.setWidth(5);
    pbar.setPen(pen);
    pbar.drawRect(10, 10, width()-20, height()-20);
    pbar.end();
}

void code128::draw_codeA(void)
{
    cnt = 0;
    flag_c = 0;
    memset(tmp_code, 0, CODE_LEN_MAX);
    QString txt_str = txt->text();
    line_txt_len = txt_str.length();    //获取lineedit 中字符长度
    if(line_txt_len > 0){    //显示文本框中条形码
        for(quint8 i=0;i<line_txt_len;i++){
            line_txt[i] = txt_str.at(i).unicode();
        }
        str_len = line_txt_len;
        opetate_code(line_txt,1);
        label->setText(txt_str);
    }
    else{
        str_len = sizeof(msg1);
        opetate_code(msg1, 1);
        label->setText("刘松虎 QQ2580065056");
    }
    update();
}

void code128::draw_codeB(void)
{
    cnt = 0;
    flag_c = 0;
    memset(tmp_code, 0, CODE_LEN_MAX);
    QString txt_str = txt->text();
    line_txt_len = txt_str.length();    //获取lineedit 中字符长度
    if(line_txt_len > 0){    //显示文本框中条形码
        for(quint8 i=0;i<line_txt_len;i++){
            line_txt[i] = txt_str.at(i).unicode();
        }
        str_len = line_txt_len;
        opetate_code(line_txt,2);
        label->setText(txt_str);
    }
    else{
        str_len = sizeof(msg2);
        opetate_code(msg2, 2);
        label->setText("刘松虎 QQ2580065056");
    }
    update();
}

void code128::draw_codeC(void)
{
    cnt = 0;
    flag_c = 1;
    memset(tmp_code, 0, CODE_LEN_MAX);
    line_txt_isnum = 1; //初始化为全为数字标志
    QString txt_str = txt->text();
    line_txt_len = txt_str.length();    //获取lineedit 中字符长度
    if(line_txt_len > 0){    //显示文本框中条形码
        for(quint8 i=0;i<line_txt_len;i++){
            line_txt[i] = txt_str.at(i).unicode();
            if(txt_str.at(i)<'0' || txt_str.at(i)>'9')
                line_txt_isnum = 0; //不全为数字
        }
        str_len = line_txt_len;
        if(1 == line_txt_isnum){
            opetate_code(line_txt,3);
        }
        else{   //不全为数字时用codeb编码
            flag_c = 0;
            opetate_code(line_txt,2);
        }
        label->setText(txt_str);
    }
    else{
        str_len = sizeof(msg3);
        opetate_code(msg3, 3);
        label->setText("刘松虎 QQ2580065056");
    }
    update();
}

char code128::opetate_code(quint8 *str, qint8 type)
{
    quint8 i, j;
    qint8 verify = 0;
    quint32 sum = 0;
    quint8 tmp[CODE_LEN_MAX];
    memset(tmp, 0, CODE_LEN_MAX);
    if(str_len > 0)
        memcpy(tmp, str, str_len);
    switch (type) {
    case 1: //start_A code A suppport number and capital letters
        make_hexarr(code_table[103]);//start_A
        for(i=0;i<str_len;i++){
            if(islower(tmp[i]))
                tmp[i] = toupper(tmp[i]);
        }
        for(sum=0,i=0;i<str_len;i++){
            make_hexarr(code_table[(tmp[i]-' ')]);
            sum += (tmp[i]-' ')*(i+1);
        }
        sum += 103; //parity=(start_value+val_ch1*1+val_ch2*2+...+val_chn*n)%103
        verify = sum % 103;
        make_hexarr(code_table[verify]);    //parity
        //qDebug("code_A sum: %d",sum);
        break;
    case 2://start_B code B support Numbers and case letters
        make_hexarr(code_table[104]); //start_B
        for(sum=0,i=0;i<str_len;i++){
            make_hexarr(code_table[(tmp[i]-' ')]);
            sum += (tmp[i]-' ')*(i+1);
        }
        sum += 104;
        verify = sum % 103;
        make_hexarr(code_table[verify]);  //parity
        //qDebug("code_B sum: %d",sum);
        break;
    case 3://start_C code C support number of 0 to 99
        if(str_len % 2){//length must to be even if length is odd,add 0 to first number
            tmp[0] = '0';
            memcpy(&tmp[1], str, str_len+1);
        }
        make_hexarr(code_table[105]);   //start_C
        for(sum=0,j=0,i=0;i<str_len;){
            if(!(isdigit(tmp[i])) && !(isdigit(tmp[i+1])))
                return 0;
            make_hexarr(code_table[((tmp[i]-'0')*10 + (tmp[i+1]-'0'))]);
            sum += (((tmp[i]-'0')*10 + (tmp[i+1]-'0')))*(j+1);
            j++;
            i += 2;
        }
        sum += 105;
        verify = sum % 103;
        make_hexarr(code_table[verify]);
        //qDebug("code_C sum: %d",sum);
        break;
    default:    //default start_B and code B
        make_hexarr((code_table[104]));
        for(i=0;i<str_len;i++){
            make_hexarr(code_table[(tmp[i]-' ')]);
        }
        //qDebug() << "code_B:" << tmp;
        break;
    }
    make_hexarr(STOP_CODE);   //end stop code is finish

    return 1;
}

void code128::make_hexarr(int code)
{
    int val = code;
    char tail = 0;
    tail = calc_tail(code);
    val = (0x01<<10) | ((code&0xff)<<2) | ((tail&0x01)<<1);
    tmp_code[cnt++] = val;
    //qDebug("code: %x",val);
}

quint8 code128::calc_tail(int code)
{
    int tmp =  code;
    char ch1 = 0;
    for(char i=0;i<8;i++){
        if(tmp&0x80)
            ch1++;
        tmp <<= 1;
    }
    return ~(ch1%2);
}
