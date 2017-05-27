#pragma once
#include "Mesh.h"
#include <fstream>
#include <string>
#include <vector>
namespace Shiny
{
class ResourceManager
{
public:
    static std::string ReadFileToString(const std::string& path);
    static bool LoadObjToMesh(const std::string& fileName, Mesh &mesh);
};
}