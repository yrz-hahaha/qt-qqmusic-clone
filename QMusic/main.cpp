#include "qmusic.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMusic w;
    w.show();
    return a.exec();
}
