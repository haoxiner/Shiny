#pragma once
#include <fstream>
#include <string>
namespace Shiny
{
class ResourceManager
{
public:
    static std::string ReadFileToString(const std::string& path);
};
}