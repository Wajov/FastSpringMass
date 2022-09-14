#include <QApplication>

#include "RenderWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    RenderWindow window(900, 900);
    window.show();

    return QApplication::exec();
}