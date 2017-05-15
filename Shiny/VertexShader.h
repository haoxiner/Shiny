#pragma once
#include "VertexAttribute.h"
#include <vector>
#include <string>
namespace Shiny
{
class VertexShader
{
public:
    bool Startup(const std::vector<VertexAttributeDescription>& vertexAttributeDescriptionList);
private:
    std::string sourceCode_;
};
}