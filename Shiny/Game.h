#pragma once
#include "RenderingSystem.h"
#include "Input.h"
#include "Mesh.h"
#include "Material.h"
#include "Entity.h"
#include "ShaderProgram.h"
#include <vector>
namespace Shiny
{
class Game
{
public:
    bool Startup(int xResolution, int yResolution, const Input* input);
    void Update(float deltaTime);
    void Render();
private:
    RenderingSystem renderingSystem_;
    ShaderProgram shaderProgram_;
    std::vector<Entity> entities_;
    std::vector<Material> materials_;
    std::vector<Mesh> meshes_;
};
}