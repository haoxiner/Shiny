#include "ResourceManager.h"

std::string Shiny::ResourceManager::ReadFileToString(const std::string& path)
{
    std::ifstream fileInputStream(path);
    if (!fileInputStream) {
        return "";
    }
    std::istreambuf_iterator<char> fileBegin(fileInputStream), fileEnd;
    return std::string(fileBegin, fileEnd);
}
