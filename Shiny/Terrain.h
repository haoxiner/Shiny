#pragma once
#include "Platform.h"
#include <string>
namespace Shiny
{
class Terrain
{
public:
    Terrain(const std::string& name);
private:
    int numOfVertexPerLine_;
    int width_;
};
}