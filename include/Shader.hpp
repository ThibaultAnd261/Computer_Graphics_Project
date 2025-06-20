#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <GL/glew.h>

class Shader {
public:
    GLuint ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use() const;

    // passer variables au shader
    void ProcessMouseMovement(float xoffset, float yoffset) const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setMat4(const std::string &name, const float* mat) const;

};

#endif