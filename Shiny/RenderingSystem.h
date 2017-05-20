#pragma once
#include "Material.h"
#include "Platform.h"
namespace Shiny
{
class RenderingSystem
{
public:
    void EnableDepthTest();
    void DisableDepthTest();
    void EnableCullFace();
    void DisableCullFace();
    void SetViewport(int x, int y, int width, int height);
    void SetMaterial(Material* material);
private:

};
}