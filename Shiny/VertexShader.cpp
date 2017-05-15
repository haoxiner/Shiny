#include "VertexShader.h"

bool Shiny::VertexShader::Startup(const std::vector<VertexAttributeDescription>& vertexAttributeDescriptionList)
{
    for (const auto& attribDesc : vertexAttributeDescriptionList) {
        sourceCode_ += attribDesc.GetFormatName() + " " + attribDesc.name + ";";
    }
    return true;
}
