#include "widget.h"
#include <QFile>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    QFile cssFile(":/style.css");
    cssFile.open(QIODevice::ReadOnly);
    a.setStyleSheet(cssFile.readAll());
    cssFile.close();
    w.show();
    return a.exec();
}
