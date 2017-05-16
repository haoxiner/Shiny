#include "VertexAttributeDescription.h"

Shiny::VertexAttributeDescription::VertexAttributeDescription(const Format format, const std::string& name) :format_(format), name_(name)
{

}

std::string Shiny::VertexAttributeDescription::GetSourceCode() const
{
    return  "in " + GetFormatName() + " " + name_ + ";";
}

std::string Shiny::VertexAttributeDescription::GetFormatName() const
{
    static const std::string formatNameList[] = {
        "float", "vec2", "vec3", "vec4",
        "int", "ivec2", "ivec3", "ivec4"
    };
    return formatNameList[format_];
}
