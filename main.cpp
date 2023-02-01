#include "ChessEntry.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChessEntry w;
    w.show();
    return a.exec();
    return 0;
}
