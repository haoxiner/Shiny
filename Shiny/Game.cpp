#include "Game.h"
#include "ResourceManager.h"
#include "FreeImage.h"
#include "TinyObjLoader.h"

#include <fstream>
#include <iostream>
#include <memory>
bool Shiny::Game::Startup(int xResolution, int yResolution, const Input* input)
{
    // OpenGL State
    renderingSystem_.EnableDepthTest();
    renderingSystem_.DisableCullFace();
    renderingSystem_.SetViewport(0, 0, xResolution, yResolution);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    meshes_.emplace_back(new Mesh(2));
    auto&& mitsuba = meshes_.back();
    ResourceManager::LoadObjToMesh("../../Resources/Model/mitsuba.obj", *mitsuba);

    bronzeMetal_.reset(new Material("bronze_copper"));
    masterRenderer_.Startup(xResolution, yResolution);

    batchOfStationaryEntity_.entityList_.emplace_back();
    auto&& e0 = batchOfStationaryEntity_.entityList_.back();
    e0.position_ = Float3(0, -2, -10);
    e0.scale_ = Float3(2.0);
    e0.models_[bronzeMetal_].emplace_back(mitsuba);
    batchOfStationaryEntity_.entityList_.emplace_back();
    auto&& e1 = batchOfStationaryEntity_.entityList_.back();
    e1.position_ = Float3(4, -2, -10);
    e1.scale_ = Float3(1.5);
    e1.models_[bronzeMetal_].emplace_back(mitsuba);
    return true;
}

void Shiny::Game::Update(float deltaTime, const Input* input)
{
    masterRenderer_.Update(deltaTime);
    Render();
}

void Shiny::Game::Render()
{
    masterRenderer_.Render(batchOfStationaryEntity_);
}

void Shiny::Game::Shutdown()
{
}
