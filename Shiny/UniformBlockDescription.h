#pragma once
#include <vector>
#include <utility>
#include <string>
namespace Shiny
{
class UniformBlockElementDescription
{
public:
    enum Format
    {
        FLOAT4, INT4, MAT4
    };
    UniformBlockElementDescription(const Format format, const std::string& name, const int arraySize);
    std::string GetSourceCode() const;
    std::string GetFormatName() const;
private:
    Format format_;
    std::string name_;
    int arraySize_;
};
class UniformBlockDescription
{
public:
    UniformBlockDescription(const std::string& name, const std::vector<UniformBlockElementDescription>& elementDescriptionList);
    std::string GetSourceCode() const;
private:
    std::string sourceCode_;
};
}