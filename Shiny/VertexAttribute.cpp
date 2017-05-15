#include "VertexAttribute.h"

const std::string& Shiny::VertexAttributeDescription::GetFormatName() const
{
    static const std::string formatNameList[] =
    {
        "float", "vec2", "vec3", "vec4",
        "int", "ivec2", "ivec3", "ivec4"
    };
    return formatNameList[format];
}
