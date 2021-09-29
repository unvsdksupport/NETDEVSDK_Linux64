#include "adudemo.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AduDemo w;
    w.show();

    return a.exec();
}
