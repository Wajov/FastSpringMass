#ifndef CLOTH_H
#define CLOTH_H

#include <cmath>
#include <vector>
#include <set>
#include <iostream>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include "Spring.h"
#include "Mesh.h"

class Cloth {
private:
    const int NUMBER_CALCULATION_ITERATION = 10, NUMBER_DEFORMATION_ITERATION = 20;
    const float STIFFNESS = 1.0f, DAMPING_FACTOR = 0.993f, DEFORMATION_LIMIT = 0.01f, WIND_POWER = 0.02f;
    const Eigen::Vector3f WIND_DIRECTION = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
    int sizeWidth, sizeHeight, numberPoints, numberSprings;
    float massPoint, h, h2;
    std::set<int> fixedPoints;
    std::vector<Spring> springs;
    Eigen::VectorXf currentPosition, previousPosition, force;
    Eigen::SparseMatrix<float> M, L, J;
    Eigen::SimplicialLLT<Eigen::SparseMatrix<float>> cholesky;
    Mesh mesh;
    void initPoints(const float mass, const float lengthWidth, const float lengthHeight);
    void initSprings(const float lengthWidth, const float lengthHeight);

public:
    Cloth(const float mass, const float length, const int size, const int fps);
    ~Cloth();
    void bind(QOpenGLShaderProgram &program);
    void update();
    void render(QOpenGLShaderProgram &program);
};

#endif