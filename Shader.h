#pragma once

#include <string>
#include <GL/glew.h>

class Shader
{
public:
    GLuint ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setMat4(const std::string& name, const float* value);
    void setInt(const std::string& name, int value) const;
private:
    std::string leerArchivoShader(const char* path);
};

