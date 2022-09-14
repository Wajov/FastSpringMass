#include "RenderWidget.h"

RenderWidget::RenderWidget(QWidget *parent, const int fps) :
        QOpenGLWidget(parent),
        press(false),
        factor(1.0f),
        cloth(0.5f, 2.0f, 33, fps),
        timer(this) {
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(1000 / fps);
}

RenderWidget::~RenderWidget() {}

void RenderWidget::initializeGL() {
    initializeOpenGLFunctions();

    program.setParent(this);
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, "shader/VertexShader.glsl")) {
        std::cerr << "Failed to add vertex shader" << std::endl;
        return;
    }
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, "shader/FragmentShader.glsl")) {
        std::cerr << "Failed to add fragment shader" << std::endl;
        return;
    }
    if (!program.link()) {
        std::cerr << "Failed to link shader program" << std::endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);
    cloth.bind(program);
}

void RenderWidget::paintGL() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.bind();

    float lightPower = 50.0f;
    QVector3D lightPosition(3.0f, 3.0f, 3.0f), cameraPosition(0.0f, 0.0f, 6.0f);
    QMatrix4x4 model, view, projection;
    model = rotate;
    model.scale(factor);
    view.lookAt(cameraPosition, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    projection.perspective(45.0f, (float)width() / (float)height(), 0.1f, 100.0f);

    program.setUniformValue("model", model);
    program.setUniformValue("view", view);
    program.setUniformValue("projection", projection);
    program.setUniformValue("lightPower", lightPower);
    program.setUniformValue("lightPosition", lightPosition);
    program.setUniformValue("cameraPosition", cameraPosition);
    cloth.render(program);
    cloth.update();
}

void RenderWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void RenderWidget::mousePressEvent(QMouseEvent *event) {
    press = true;
    lastX = event->x();
    lastY = event->y();
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event) {
    press = false;
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event) {
    if (press) {
        QVector3D a = QVector3D((float)lastX / (float)width() - 0.5f, 0.5f - (float)lastY / (float)height(), 1.0f).normalized();
        QVector3D b = QVector3D((float)event->x() / (float)width() - 0.5f, 0.5f - (float)event->y() / (float)height(), 1.0f).normalized();
        QVector3D axis = QVector3D::crossProduct(a, b);
        float angle = std::acos(QVector3D::dotProduct(a, b));
        QMatrix4x4 temp;
        temp.rotate(angle / (float)M_PI * 1800.0f, axis);
        rotate = temp * rotate;
    }

    lastX = event->x();
    lastY = event->y();
    update();
}

void RenderWidget::wheelEvent(QWheelEvent *event) {
    factor += (float)event->angleDelta().y() * 1e-3f;
    factor = std::max(factor, 0.01f);
    update();
}