#include "RenderWindow.h"

RenderWindow::RenderWindow(const int width, const int height) :
    QWidget(),
    horizontal(this),
    widget(this, 120) {
    horizontal.addWidget(&widget);
    setLayout(&horizontal);
    resize(width, height);
}

RenderWindow::~RenderWindow() {}