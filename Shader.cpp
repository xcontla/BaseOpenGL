#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = leerArchivoShader(vertexPath);
    std::string fragmentCode = leerArchivoShader(fragmentPath);
    std::cout << vertexCode << std::endl;
    std::cout << fragmentCode << std::endl;

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    GLint codeLength[1];
    codeLength[0] = strlen(vShaderCode);

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, codeLength);
    glCompileShader(vertex);

    GLint success;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar eLog[1024] = { 0 };
        glGetShaderInfoLog(vertex, sizeof(eLog), NULL, eLog);
        std::cerr << "Error compilando Vertex Shader: " << eLog << std::endl;
    }

    codeLength[0] = strlen(fShaderCode);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, codeLength);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar eLog[1024] = { 0 };
        glGetShaderInfoLog(vertex, sizeof(eLog), NULL, eLog);
        std::cerr << "Error compilando Fragment Shader: " << eLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(ID, sizeof(eLog), NULL, eLog);
        printf("ERROR linking program: '%s'\n", eLog);
        return;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setMat4(const std::string& name, const float* value) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

std::string Shader::leerArchivoShader(const char* path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
