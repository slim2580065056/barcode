#include "widget.h"
#include <QApplication>
#include "code128.h"
#include "qrcode.h"


#define test    0

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if test
    Widget w;
    w.show();
#else
    code128 bar;
    bar.show();
    QRcode qr;
    qr.show();
#endif

    return a.exec();
}
