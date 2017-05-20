#pragma once
#include "Platform.h"
#include <string>
namespace Shiny
{
class GLSLProgram
{
public:
    bool Startup(const std::string& vertexShaderSource,
                 const std::string& fragmentShaderSource);

private:
    GLuint Shiny::GLSLProgram::CreateShader(const std::string& source, GLenum type);
    GLuint id_;
};
}