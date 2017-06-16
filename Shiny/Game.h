#pragma once
#include "Input.h"
#include "Mesh.h"
#include "Material.h"
#include "Entity.h"
#include "ThirdPersonCamera.h"
#include "ShaderProgram.h"
#include "MasterRenderer.h"
#include "Cubemap.h"
#include "Terrain.h"
#include "Camera.h"
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
    ThirdPersonCamera thirdPersonCamera_;
    std::vector<std::shared_ptr<Mesh>> meshes_;
    std::shared_ptr<Material> bronzeMetal_;
    std::unique_ptr<SkyBox> skyBox_;
    std::unique_ptr<Terrain> terrain_;
    MasterRenderer masterRenderer_;
    BatchOfStationaryEntity batchOfStationaryEntity_;
    BatchOfAnimatedEntity batchOfAnimatedEntity_;
    std::shared_ptr<Animation> animation_;
};
}