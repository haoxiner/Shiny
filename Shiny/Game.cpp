#include "Game.h"
#include "ResourceManager.h"
#include "FreeImage.h"
#include "TinyObjLoader.h"
#include "ThirdPersonCamera.h"

#include <fstream>
#include <iostream>
#include <memory>
bool Shiny::Game::Startup(int xResolution, int yResolution, const Input* input)
{
    masterRenderer_.Startup(xResolution, yResolution);

    meshes_.emplace_back(std::make_shared<Mesh>(1));
    //ResourceManager::LoadObjToMesh("../../Resources/Model/mitsuba.obj", *meshes_[0]);
    meshes_[0]->LoadStandardPackage("prototype");

    meshes_.emplace_back(std::make_shared<Mesh>(1));
    meshes_[1]->LoadStandardPackage("arena");

    bronzeMetal_.reset(new Material("bronze_copper"));
    
    animation_.reset(new Animation("prototype"));

    playerEntity_.reset(new Entity);
    batchOfAnimatingEntity_.batch_[animation_].emplace_back(playerEntity_);
    //auto& e0 = batchOfAnimatingEntity_.batch_[animation_].back();
    playerEntity_->position_ = Float3(63.5, 63.5, 0);
    playerEntity_->scale_ = Float3(0.01);
    playerEntity_->models_[bronzeMetal_].emplace_back(meshes_[0]);
    playerEntity_->rotation_ = Float4(0,0,1,0);

    batchOfStationaryEntity_.entityList_.emplace_back(new Entity);
    auto& e1 = batchOfStationaryEntity_.entityList_.back();
    e1->position_ = Float3(0);
    e1->scale_ = Float3(100);
    e1->rotation_ = Float4(0, 0, 1, 1);
    e1->models_[bronzeMetal_].emplace_back(meshes_[1]);

    //batchOfStationaryEntity_.entityList_.emplace_back();
    //auto& e1 = batchOfStationaryEntity_.entityList_.back();
    //e1.position_ = Float3(4, -2, -10);
    //e1.scale_ = Float3(1.5);
    //e1.models_[bronzeMetal_].emplace_back(meshes_[1]);
    
    skyBox_.reset(new SkyBox("uffizi"));
    terrain_.reset(new Terrain(""));
    return true;
}

void Shiny::Game::Update(float deltaTime, const Input* input)
{
    thirdPersonCamera_.AddForce(-input->GetRightHorizontalAxis(), input->GetRightVerticalAxis());
    masterRenderer_.Update(deltaTime);
    Matrix4x4 view;
    Float3 position;
    thirdPersonCamera_.GetPose(view, position, playerEntity_->position_ + Float3(0,0,1.5));
    masterRenderer_.SetCameraPose(view, position);
    Render();
}

void Shiny::Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    masterRenderer_.Render(batchOfAnimatingEntity_);
    //masterRenderer_.Render(batchOfStationaryEntity_);
    masterRenderer_.Render(*terrain_);
    masterRenderer_.RenderSky(*skyBox_);
}

void Shiny::Game::Shutdown()
{
}
