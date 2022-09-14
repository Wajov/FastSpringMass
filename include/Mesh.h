#ifndef MESH_H
#define MESH_H

#include <cfloat>
#include <vector>

#include <Eigen/Dense>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QString>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include "Vertex.h"

class Mesh : protected QOpenGLFunctions {
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    QString texturePath;
    QOpenGLTexture *texture;
    QOpenGLVertexArrayObject *vao;
    QOpenGLBuffer vbo, ebo;

public:
    Mesh(const int sizeWidth, const int sizeHeight, const QString& texturePath);
    ~Mesh();
    QVector3D normal(const int index);
    void bind(QOpenGLShaderProgram &program);
    void setPosition(const Eigen::VectorXf& position);
    void update(const Eigen::VectorXf& position);
    void render(QOpenGLShaderProgram &program);
};

#endif