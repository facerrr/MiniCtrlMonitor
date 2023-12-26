#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    QFile file(":/qsslib/test.qss");
//    if (file.open(QFile::ReadOnly))
//    {
//        QTextStream filetext(&file);
//        QString stylesheet = filetext.readAll();
//        qApp->setStyleSheet(stylesheet);
//    }
    w.show();
    return a.exec();
}
