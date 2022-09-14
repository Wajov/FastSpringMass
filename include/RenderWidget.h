#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <climits>
#include <cmath>
#include <iostream>

#include <QVector3D>
#include <QMatrix4x4>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>

#include "Cloth.h"

class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
private:
    bool press;
    int lastX, lastY;
    float factor;
    QMatrix4x4 rotate;
    Cloth cloth;
    QOpenGLShaderProgram program;
    QTimer timer;
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

public:
    RenderWidget(QWidget *parent, const int fps);
    ~RenderWidget() override;
};

#endif