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
    const std::string& GetFormatName() const;
    Format format;
    std::string name;
};
}