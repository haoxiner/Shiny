#include "Game.h"
#include "ResourceManager.h"
#include "FreeImage.h"
#include "TinyObjLoader.h"

#include <fstream>
#include <iostream>
#include <memory>
bool Shiny::Game::Startup(int xResolution, int yResolution, const Input* input)
{
    masterRenderer_.Startup(xResolution, yResolution);

    meshes_.emplace_back(std::make_shared<Mesh>(1));
    //ResourceManager::LoadObjToMesh("../../Resources/Model/mitsuba.obj", *meshes_[0]);
    meshes_[0]->LoadStandardPackage("mitsuba");

    bronzeMetal_.reset(new Material("bronze_copper"));
    
    batchOfStationaryEntity_.entityList_.emplace_back();
    auto& e0 = batchOfStationaryEntity_.entityList_.back();
    e0.position_ = Float3(0, -2, -5);
    e0.scale_ = Float3(2.0);
    e0.models_[bronzeMetal_].emplace_back(meshes_[0]);

    //batchOfStationaryEntity_.entityList_.emplace_back();
    //auto& e1 = batchOfStationaryEntity_.entityList_.back();
    //e1.position_ = Float3(4, -2, -10);
    //e1.scale_ = Float3(1.5);
    //e1.models_[bronzeMetal_].emplace_back(meshes_[1]);
    
    skyBox_.reset(new SkyBox("uffizi"));
   
    return true;
}

void Shiny::Game::Update(float deltaTime, const Input* input)
{
    masterRenderer_.Update(deltaTime);
    Render();
}

void Shiny::Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    masterRenderer_.Render(batchOfStationaryEntity_);
    masterRenderer_.RenderSky(*skyBox_);
}

void Shiny::Game::Shutdown()
{
}
