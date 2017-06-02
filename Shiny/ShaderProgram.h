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
    bool Startup(const std::string& computeShaderSource, int localSizeX, int localSizeY, int localSizeZ);
    void Use();
    void Shutdown();
    GLuint GetID() const { return program_; }
    static GLuint LoadShader(const std::string& source, GLenum type);
private:
    GLuint program_;
};
}