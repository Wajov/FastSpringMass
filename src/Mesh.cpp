#include "Mesh.h"

Mesh::Mesh(const int sizeWidth, const int sizeHeight, const QString& texturePath) :
    vertices(sizeWidth * sizeHeight),
    texturePath(texturePath) {
    for (int i = 0; i < sizeHeight; i++)
        for (int j = 0; j < sizeWidth; j++)
            vertices[i * sizeWidth + j].uv = QVector2D((float)j / float(sizeWidth - 1), (float)i / (float)(sizeHeight - 1));

    for (int i = 0; i < sizeHeight - 1; i++)
        for (int j = 0; j < sizeWidth - 1; j++) {
            int p00 = sizeWidth * i + j;
            int p01 = sizeWidth * i + j + 1;
            int p10 = sizeWidth * (i + 1) + j;
            int p11 = sizeWidth * (i + 1) + j + 1;
            indices.push_back(p00);
            indices.push_back(p10);
            indices.push_back(p01);
            indices.push_back(p11);
            indices.push_back(p01);
            indices.push_back(p10);
        }
}

Mesh::~Mesh() {
    delete texture;
    delete vao;
}

QVector3D Mesh::normal(const int index) {
    return vertices[index].normal;
}

void Mesh::bind(QOpenGLShaderProgram &program) {
    initializeOpenGLFunctions();

    vao = new QOpenGLVertexArrayObject();
    QOpenGLVertexArrayObject::Binder binder(vao);

    vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo.create();
    vbo.bind();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(Vertex));

    ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    ebo.create();
    ebo.bind();
    ebo.allocate(indices.data(), indices.size() * sizeof(unsigned int));

    int attributePosition = program.attributeLocation("position");
    program.setAttributeBuffer(attributePosition, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));
    program.enableAttributeArray(attributePosition);

    int attributeNormal = program.attributeLocation("normal");
    program.setAttributeBuffer(attributeNormal, GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));
    program.enableAttributeArray(attributeNormal);

    int attributeUV = program.attributeLocation("uv");
    program.setAttributeBuffer(attributeUV, GL_FLOAT, offsetof(Vertex, uv), 2, sizeof(Vertex));
    program.enableAttributeArray(attributeUV);

    texture = new QOpenGLTexture(QImage(texturePath));
    texture->setWrapMode(QOpenGLTexture::Repeat);
    texture->setMinificationFilter(QOpenGLTexture::Linear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->generateMipMaps();
}

void Mesh::setPosition(const Eigen::VectorXf &position) {
    for (int i = 0; i < vertices.size(); i++)
        vertices[i].position = QVector3D(position(i * 3), position(i * 3 + 1), position(i * 3 + 2));

    std::vector<std::vector<QVector3D>> normals(vertices.size());
    for (auto iter = indices.begin(); iter != indices.end();) {
        unsigned int x = *(iter++);
        unsigned int y = *(iter++);
        unsigned int z = *(iter++);
        QVector3D a = vertices[y].position - vertices[x].position;
        QVector3D b = vertices[z].position - vertices[y].position;
        QVector3D normal = QVector3D::crossProduct(a, b).normalized();
        normals[x].push_back(normal);
        normals[y].push_back(normal);
        normals[z].push_back(normal);
    }

    for (int i = 0; i < vertices.size(); i++)
        if (!normals[i].empty())
            vertices[i].normal = (std::accumulate(normals[i].begin(), normals[i].end(), QVector3D(0.0f, 0.0f, 0.0f)) / (float) normals[i].size()).normalized();
}

void Mesh::update(const Eigen::VectorXf &position) {
    setPosition(position);

    vbo.bind();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(Vertex));
}

void Mesh::render(QOpenGLShaderProgram &program) {
    QOpenGLVertexArrayObject::Binder binder(vao);
    texture->bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}