#include "Game.h"
#include "ResourceManager.h"
#include <fstream>
bool Shiny::Game::Startup(int xResolution, int yResolution, const Input* input)
{
    renderingSystem_.DisableDepthTest();
    renderingSystem_.EnableCullFace();
    renderingSystem_.SetViewport(0, 0, xResolution, yResolution);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    std::vector<short> positions = {
        MapToShort(0.5f), MapToShort(0.5f), MapToShort(0.0f), MapToShort(1.0f),
        MapToShort(-0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(1.0f)
    };
    std::vector<short> normals = {
        MapToShort(0.5f), MapToShort(0.5f), MapToShort(0.0f), MapToShort(1.0f),
        MapToShort(-0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(0.0f)
    };
    std::vector<unsigned int> indices = { 0,1,2 };
    meshes_.emplace_back(2);
    auto&& mesh = meshes_.back();
    mesh.LoadVertexAttribute(0, 4, true, positions);
    mesh.LoadVertexAttribute(1, 4, true, normals);
    mesh.LoadIndices(indices);

    shaderProgram_.Startup(ResourceManager::ReadFileToString("./Shaders/PBR.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));
    return true;
}

void Shiny::Game::Update(float deltaTime)
{
}

void Shiny::Game::Render()
{
    shaderProgram_.Use();
    for (auto&& mesh : meshes_) {
        mesh.Render();
    }
}
