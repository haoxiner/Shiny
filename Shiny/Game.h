#pragma once
#include "RenderingSystem.h"
#include "Input.h"
#include "Mesh.h"
#include "Material.h"
#include "Entity.h"
#include "ShaderProgram.h"
#include "MasterRenderer.h"
#include "Cubemap.h"
#include "MathUtil.h"
#include <vector>
namespace Shiny
{
class Game
{
public:
    bool Startup(int xResolution, int yResolution, const Input* input);
    void Update(float deltaTime, const Input* input);
    void Render();
    void Shutdown();
private:
    RenderingSystem renderingSystem_;
    std::vector<std::shared_ptr<Mesh>> meshes_;
    std::shared_ptr<Material> bronzeMetal_;
    MasterRenderer masterRenderer_;
    BatchOfStationaryEntity batchOfStationaryEntity_;
};
}