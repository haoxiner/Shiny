#include "ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <vector>

Shiny::ShaderProgram::ShaderProgram()
{
    program_ = glCreateProgram();
}

bool Shiny::ShaderProgram::Startup(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    auto vertexShader = LoadShader(vertexShaderSource, GL_VERTEX_SHADER);
    auto fragmentShader = LoadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);
    glDetachShader(program_, vertexShader);
    glDetachShader(program_, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    GLint success = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        GLint logLength;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> infoLog(logLength);
        glGetProgramInfoLog(program_, logLength, &logLength, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }
    return true;
}

bool Shiny::ShaderProgram::Startup(const std::string& computeShaderSource) {
    auto computeShader = LoadShader(computeShaderSource, GL_COMPUTE_SHADER);
    program_ = glCreateProgram();
    glAttachShader(program_, computeShader);
    glLinkProgram(program_);
    glDetachShader(program_, computeShader);
    glDeleteShader(computeShader);
    GLint success = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        GLint logLength;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> infoLog(logLength);
        glGetProgramInfoLog(program_, logLength, &logLength, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }
    int size[3];
    glGetProgramiv(program_, GL_COMPUTE_WORK_GROUP_SIZE, size);
    std::cerr << "Work group size is " << size[0] << "x" << size[1] << "x" << size[2] << std::endl;
    return true;
}

void Shiny::ShaderProgram::Use() {
    glUseProgram(program_);
}

void Shiny::ShaderProgram::Shutdown() {
    glDeleteProgram(program_);
}

GLuint Shiny::ShaderProgram::LoadShader(const std::string& source, GLenum type) {
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
