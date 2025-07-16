#include "countdownwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CountdownWindow w;
    w.show();
    return a.exec();
}
