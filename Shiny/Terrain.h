#pragma once
#include "Material.h"
#include "Mesh.h"
#include "Platform.h"
#include <string>
#include <memory>
namespace Shiny
{
class Terrain
{
public:
    Terrain(const std::string& name);
    void Render() const;
private:
    int numOfVertexPerLine_;
    int width_;
    std::unique_ptr<Material> material_;
    std::unique_ptr<Mesh> mesh_;
};
}