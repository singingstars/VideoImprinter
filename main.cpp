#include "videoimprinter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VideoImprinter w;
    w.show();
    
    return a.exec();
}
