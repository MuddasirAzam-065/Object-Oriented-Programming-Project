#include <QApplication>
#include "lms.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    lms w;

    w.showMaximized();

    return a.exec();
}
