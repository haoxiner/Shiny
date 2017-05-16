#include "GLSLProgram.h"
#include <fstream>
#include <iostream>
bool Shiny::GLSLProgram::Startup(const std::vector<VertexAttributeDescription>& vertexAttributeDescriptionList,
                                 const std::vector<UniformBlockDescription>& uniformBlockDescriptionList,
                                 const std::string& vertexShaderMain,
                                 const std::string& pixelShaderMain)
{
    std::string vertexShaderSource = "#version 450 core\n";
    for (int i = 0; i < vertexAttributeDescriptionList.size(); i++) {
        const auto& attribDesc = vertexAttributeDescriptionList[i];
        vertexShaderSource += "layout(location=" + std::to_string(i) + ") " + attribDesc.GetSourceCode() +"\n";
    }
    std::string pixelShaderSource = "#version 450 core\n";
    for (int i = 0; i < uniformBlockDescriptionList.size(); i++) {
        const auto& uniformBlockDescription = uniformBlockDescriptionList[i];
        auto uniformBlockSourceCode = "layout(binding=" + std::to_string(i) + ",std140) " + uniformBlockDescription.GetSourceCode() + "\n";
        vertexShaderSource += uniformBlockSourceCode;
        pixelShaderSource += uniformBlockSourceCode;
    }
    std::cerr << "Output VS/PS to files." << std::endl;
    std::ofstream vs("G:/haoxin/VS.glsl");
    vs << vertexShaderSource << vertexShaderMain << "\nvoid main() { VS(); }";
    vs.close();
    std::ofstream ps("G:/haoxin/PS.glsl");
    ps << pixelShaderSource << pixelShaderMain << "\nout vec4 pixelColor;\nvoid main() { pixelColor = PS(); }";
    ps.close();

    auto vertexShader = LoadShader(vertexShaderSource, GL_VERTEX_SHADER);
    auto pixelShader = LoadShader(pixelShaderSource, GL_FRAGMENT_SHADER);
    id_ = glCreateProgram();
    if (id_ == 0) {
        return false;
    }
    glAttachShader(id_, vertexShader);
    glAttachShader(id_, pixelShader);
    glLinkProgram(id_);
    glDetachShader(id_, vertexShader);
    glDetachShader(id_, pixelShader);
    glDeleteShader(vertexShader);
    glDeleteShader(pixelShader);
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

GLuint Shiny::GLSLProgram::LoadShader(const std::string& source, GLenum type)
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