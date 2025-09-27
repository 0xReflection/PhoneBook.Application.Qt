#include "PhoneBookAppQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PhoneBookAppQt window;
    window.show();
    return app.exec();
}
