#include "fsmeditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FsmEditor w;
    w.show();

    return a.exec();
}
