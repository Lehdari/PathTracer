#ifndef SHADER_HPP
#define SHADER_HPP


#include "MathTypes.hpp"

#include <string>
#include <GL/glew.h>


class Shader {
public:
    Shader(const std::string& vsFileName, const std::string& fsFileName);

    GLuint getId(void) const;
    void useShader(const Matrix4Glf& mvp,
                   const Vector3Glf& color = Vector3Glf(1.0f, 1.0f, 1.0f)) const;

private:
    GLuint programId_;
    GLuint uniformPosition_MVP_;
    GLuint uniformPosition_Color_;
};


#endif // SHADER_HPP
