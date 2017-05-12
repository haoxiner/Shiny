#pragma once
#include "Model.h"
#include "Material.h"
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
    void Render(Model* model);
private:

};
}