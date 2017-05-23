#pragma once
#include "Platform.h"
#include "MathUtil.h"
#include <string>
namespace Shiny
{
class ShaderProgram
{
public:
    ShaderProgram();
    bool Startup(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    bool Startup(const std::string& computeShaderSource);
    void Use();
    void Shutdown();
    GLuint GetID() const { return program_; }
private:
    GLuint LoadShader(const std::string& source, GLenum type);
    GLuint program_;
};
}