#pragma once
#include "Mesh.h"
#include "Cubemap.h"
#include <memory>
namespace Shiny
{
class SkyBox
{
public:
    SkyBox();
private:
    Mesh mesh_;
    std::unique_ptr<Cubemap> cubemap_;
};
}