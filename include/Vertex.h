#ifndef VERTEX_H
#define VERTEX_H

#include <QVector2D>
#include <QVector3D>

class Vertex {
public:
    QVector3D position, normal;
    QVector2D uv;
    Vertex();
    ~Vertex();
};

#endif