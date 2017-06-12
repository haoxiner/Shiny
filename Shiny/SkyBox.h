#pragma once
#include "Mesh.h"
#include "Cubemap.h"
#include <string>
#include <memory>
namespace Shiny
{
class SkyBox
{
public:
    SkyBox(const std::string& name);
    void Render();
private:
    static std::unique_ptr<Mesh> mesh_;
    std::unique_ptr<Cubemap> cubemap_;
};
}