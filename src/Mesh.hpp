#ifndef MESH_HPP
#define MESH_HPP


#include "Shader.hpp"
#include "BasicCamera.hpp"
#include "MathTypes.hpp"

#include <string>
#include <GL/glew.h>


class Mesh {
public:
    Mesh(void);
    ~Mesh(void);

    Mesh(const Mesh& other)             = delete;
    Mesh(Mesh&& other)                  = delete;
    Mesh& operator=(const Mesh& other)  = delete;
    Mesh& operator=(Mesh&& other)       = delete;

    void loadFromObj(const std::string& fileName);
    void render(const Shader& shader,
                const BasicCamera& camera,
                const Vector3Glf& color = Vector3Glf(1.0f, 1.0f, 1.0f)) const;

    void setPosition(const Vector3Glf& position);
    void setRotation(const Matrix3Glf& rotation);

    Matrix4Glf getOrientation(void) const;

private:
    GLuint vertexArrayObjectId_;
    unsigned nIndices_;

    GLuint positionBufferId_;
    GLuint texCoordBufferId_;
    GLuint normalBufferId_;
    GLuint elementBufferId_;

    Vector3Glf position_;
    Matrix3Glf rotation_;
};


#endif // MESH_HPP
