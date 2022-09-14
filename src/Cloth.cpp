#include "Cloth.h"

Cloth::Cloth(const float mass, const float length, const int size, const int fps) :
    sizeWidth(2 * size - 1),
    sizeHeight(size),
    h(1.0f / (float)fps),
    h2(h * h),
    mesh(sizeWidth, sizeHeight, "flag/soviet_union.png") {
    initPoints(mass, 2.0f * length, length);
    initSprings(2.0f * length, length);
    cholesky.compute(M + h2 * L);
}

Cloth::~Cloth() {}

void Cloth::initPoints(const float mass, const float lengthWidth, const float lengthHeight) {
    numberPoints = sizeWidth * sizeHeight;
    massPoint = mass / (float)numberPoints;

    fixedPoints.insert(0);
    fixedPoints.insert((sizeHeight - 1) * sizeWidth);

    float d = lengthWidth / (float)(sizeWidth - 1);
    int k = 0;
    currentPosition.resize(3 * numberPoints);
    for (int i = 0; i < sizeHeight; i++)
        for (int j = 0; j < sizeWidth; j++) {
            currentPosition(k++) = -0.5f * lengthWidth;
            currentPosition(k++) = 0.5f * lengthHeight - d * (float)i;
            currentPosition(k++) = d * (float)j;
        }
    previousPosition = currentPosition;

    force.resize(3 * numberPoints);

    M.resize(3 * numberPoints, 3 * numberPoints);
    for (int i = 0; i < numberPoints; i++)
        for (int j = 0; j < 3; j++)
            M.insert(3 * i + j, 3 * i + j) = massPoint;

    mesh.setPosition(currentPosition);
}

void Cloth::initSprings(const float lengthWidth, const float lengthHeight) {
    float root2 = std::sqrt(2.0f), springLength = 1.05f * lengthWidth / (float)(sizeWidth - 1);
    for (int i = 0; i < sizeHeight; i++)
        for (int j = 0; j < sizeWidth; j++) {
            if (i == sizeHeight - 1 && j == sizeWidth - 1)
                continue;

            if (i == sizeHeight - 1) {
                springs.emplace_back(sizeWidth * i + j, sizeWidth * i + j + 1, springLength);
                if (j % 2 == 0)
                    springs.emplace_back(sizeWidth * i + j, sizeWidth * i + j + 2, 2.0f * springLength);
                continue;
            }

            if (j == sizeWidth - 1) {
                springs.emplace_back(sizeWidth * i + j, sizeWidth * (i + 1) + j, springLength);
                if (i % 2 == 0)
                    springs.emplace_back(sizeWidth * i + j, sizeWidth * (i + 2) + j, 2.0f * springLength);
                continue;
            }

            springs.emplace_back(sizeWidth * i + j, sizeWidth * i + j + 1, springLength);
            springs.emplace_back(sizeWidth * i + j, sizeWidth * (i + 1) + j, springLength);
            springs.emplace_back(sizeWidth * i + j, sizeWidth * (i + 1) + j + 1, root2 * springLength);
            springs.emplace_back(sizeWidth * (i + 1) + j, sizeWidth * i + j + 1, root2 * springLength);
            if (j % 2 == 0)
                springs.emplace_back(sizeWidth * i + j, sizeWidth * i + j + 2, 2.0f * springLength);
            if (i % 2 == 0)
                springs.emplace_back(sizeWidth * i + j, sizeWidth * (i + 2) + j, 2.0f * springLength);
        }
    numberSprings = springs.size();

    L.resize(3 * numberPoints, 3 * numberPoints);
    J.resize(3 * numberPoints, 3 * numberSprings);
    std::vector<Eigen::Triplet<float>> lTriplets, jTriplets;
    for (int i = 0; i < numberSprings; i++) {
        int source = springs[i].getSource();
        int target = springs[i].getTarget();
        for (int j = 0; j < 3; j++) {
            lTriplets.emplace_back(3 * source + j, 3 * source + j, STIFFNESS);
            lTriplets.emplace_back(3 * source + j, 3 * target + j, -STIFFNESS);
            lTriplets.emplace_back(3 * target + j, 3 * source + j, -STIFFNESS);
            lTriplets.emplace_back(3 * target + j, 3 * target + j, STIFFNESS);
            jTriplets.emplace_back(3 * source + j, 3 * i + j, STIFFNESS);
            jTriplets.emplace_back(3 * target + j, 3 * i + j, -STIFFNESS);
        }
    }
    L.setFromTriplets(lTriplets.begin(), lTriplets.end());
    J.setFromTriplets(jTriplets.begin(), jTriplets.end());
}

void Cloth::bind(QOpenGLShaderProgram &program) {
    mesh.bind(program);
}

void Cloth::update() {
    for (int i = 0; i < numberPoints; i++) {
        QVector3D normalTemp = mesh.normal(i);
        Eigen::Vector3f normal(normalTemp.x(), normalTemp.y(), normalTemp.z());
        Eigen::Vector3f wind = WIND_POWER * std::fabs(normal.dot(WIND_DIRECTION)) * WIND_DIRECTION;
        Eigen::Vector3f gravity(0.0f, -9.8f * massPoint, 0.0f);
        Eigen::Vector3f forcePoint = wind + gravity;
        for (int j = 0; j < 3; j++)
            force(i * 3 + j) = forcePoint(j);
    }

    Eigen::VectorXf My = M * ((DAMPING_FACTOR + 1.0f) * currentPosition - DAMPING_FACTOR * previousPosition);
    previousPosition = currentPosition;
    for (int i = 0; i < NUMBER_CALCULATION_ITERATION; i++) {
        Eigen::VectorXf d(3 * numberSprings);
        for (int j = 0; j < numberSprings; j++) {
            int source = springs[j].getSource();
            int target = springs[j].getTarget();
            float length = springs[j].getLength();
            Eigen::Vector3f p;
            for (int k = 0; k < 3; k++)
                p(k) = currentPosition(3 * source + k) - currentPosition(3 * target + k);
            p.normalize();
            for (int k = 0; k < 3; k++)
                d(3 * j + k) = length * p(k);
        }

        Eigen::VectorXf b = My + h2 * (J * d + force);
        currentPosition = cholesky.solve(b);
    }

    for (int i = 0; i < NUMBER_DEFORMATION_ITERATION; i++)
        for (const Spring& spring : springs) {
            int source = spring.getSource();
            int target = spring.getTarget();
            float length = spring.getLength();
            Eigen::Vector3f p;
            for (int j = 0; j < 3; j++)
                p(j) = currentPosition(3 * source + j) - currentPosition(3 * target + j);

            float exactLength = p.norm();
            if ((exactLength - length) / length > DEFORMATION_LIMIT) {
                float f1, f2, diff = (exactLength - (1 + DEFORMATION_LIMIT) * length) / exactLength;
                bool sourceFixed = (fixedPoints.find(source) != fixedPoints.end());
                bool targetFixed = (fixedPoints.find(target) != fixedPoints.end());
                if (sourceFixed && targetFixed)
                    f1 = f2 = 0.0f;
                else if (sourceFixed) {
                    f1 = 0.0f;
                    f2 = 1.0f;
                } else if (targetFixed) {
                    f1 = 1.0f;
                    f2 = 0.0f;
                } else
                    f1 = f2 = 0.5f;

                for (int j = 0; j < 3; j++) {
                    currentPosition(3 * source + j) -= p(j) * f1 * diff;
                    currentPosition(3 * target + j) += p(j) * f2 * diff;
                }
            }
        }

    for (const int point : fixedPoints)
        for (int i = 0; i < 3; i++)
        currentPosition(3 * point + i) = previousPosition(3 * point + i);

    mesh.update(currentPosition);
}

void Cloth::render(QOpenGLShaderProgram &program) {
    mesh.render(program);
}