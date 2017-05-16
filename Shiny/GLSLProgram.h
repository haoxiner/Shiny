#pragma once
#include "VertexAttributeDescription.h"
#include "UniformBlockDescription.h"
#include "Platform.h"
#include <vector>
#include <utility>
#include <string>
namespace Shiny
{
class GLSLProgram
{
public:
    bool Startup(const std::vector<VertexAttributeDescription>& vertexAttributeDescriptionList,
                 const std::vector<UniformBlockDescription>& uniformBlockDescriptionList,
                 const std::string& vertexShaderMain,
                 const std::string& pixelShaderMain);
private:
    GLuint Shiny::GLSLProgram::LoadShader(const std::string& source, GLenum type);
    GLuint id_;
};
}