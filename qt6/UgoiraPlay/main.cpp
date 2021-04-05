#include "ugoiraview.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UgoiraView w;
    w.show();
    return a.exec();
}
