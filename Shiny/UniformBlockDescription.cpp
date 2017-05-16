#include "UniformBlockDescription.h"

Shiny::UniformBlockElementDescription::UniformBlockElementDescription(const Format format,
                                                                      const std::string& name,
                                                                      const int arraySize) :format_(format), name_(name), arraySize_(arraySize)
{
}

std::string Shiny::UniformBlockElementDescription::GetSourceCode() const
{
    return GetFormatName() + " " + name_ + ";";
}

std::string Shiny::UniformBlockElementDescription::GetFormatName() const
{
    static const std::string formatNameList[] = {
        "vec4", "ivec4", "mat4"
    };
    std::string name = formatNameList[format_];
    if (arraySize_ > 0) {
        name += "[" + std::to_string(arraySize_) + "]";
    }
    return name;
}

Shiny::UniformBlockDescription::UniformBlockDescription(const std::string& name,
                                                        const std::vector<UniformBlockElementDescription>& elementDescriptionList)
{
    sourceCode_ = "uniform " + name + " {\n";
    for (int i = 0; i < elementDescriptionList.size(); i++) {
        const auto& elementDescription = elementDescriptionList[i];
        sourceCode_ += "    " + elementDescription.GetSourceCode() + "\n";
    }
    sourceCode_ += "};";
}

std::string Shiny::UniformBlockDescription::GetSourceCode() const
{
    return sourceCode_;
}
