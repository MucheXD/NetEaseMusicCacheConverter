#include "ucConverter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NMTC w;
    w.show();
    return a.exec();
}
