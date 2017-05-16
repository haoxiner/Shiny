#pragma once
#include "MathUtil.h"
#include <string>
namespace Shiny
{
class VertexAttributeDescription
{
public:
    enum Format
    {
        FLOAT, FLOAT2, FLOAT3, FLOAT4,
        INT, INT2, INT3, INT4
    };
    VertexAttributeDescription(const Format format, const std::string& name);
    std::string GetSourceCode() const;
    std::string GetFormatName() const;
private:
    Format format_;
    std::string name_;
};
}