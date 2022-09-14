#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <string>

#include <QMainWindow>
#include <QHBoxLayout>

#include "RenderWidget.h"

class RenderWindow : public QWidget {
    Q_OBJECT
private:
    QHBoxLayout horizontal;
    RenderWidget widget;

public:
    RenderWindow(const int width, const int height);
    ~RenderWindow() override;
};

#endif