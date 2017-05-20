#include "GLSLProgram.h"
#include <fstream>
#include <iostream>
#include <vector>
bool Shiny::GLSLProgram::Startup(const std::string& vertexShaderSource,
                                 const std::string& fragmentShaderSource)
{
    auto vertexShader = CreateShader(vertexShaderSource, GL_VERTEX_SHADER);
    auto fragmentShader = CreateShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    id_ = glCreateProgram();
    if (id_ == 0) {
        return false;
    }
    glAttachShader(id_, vertexShader);
    glAttachShader(id_, fragmentShader);
    glLinkProgram(id_);
    glDetachShader(id_, vertexShader);
    glDetachShader(id_, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    GLint success = GL_FALSE;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        GLint logLength;
        glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> infoLog(logLength);
        glGetProgramInfoLog(id_, logLength, &logLength, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
        return false;
    }
    return true;
}

GLuint Shiny::GLSLProgram::CreateShader(const std::string& source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    const char* const sourcePointer = source.c_str();
    glShaderSource(shader, 1, &sourcePointer, nullptr);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> infoLog(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }
    return shader;
}