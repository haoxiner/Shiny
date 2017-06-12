#pragma once
#include "BatchOfStationaryEntity.h"
#include "Cubemap.h"
#include "ShaderProgram.h"
#include "MathUtil.h"
#include "Platform.h"
#include <string>
#include <memory>
namespace Shiny
{
class MasterRenderer
{
public:
    bool Startup(int xResolution, int yResolution);
    void Shutdown();
    void SetupEnvironment(const std::string& name);
    void Update(float deltaTime) { deltaTime_ = deltaTime; }
    void Render(BatchOfStationaryEntity& batch);
private:
    float deltaTime_ = 0.0f;
private:
    ShaderProgram stationaryEntityShader_;
    GLuint dfgTextureID_;
    GLuint defaultSamplerID_;
    GLuint repeatSamplerID_;
    std::vector<GLuint> constantBufferList_;
    std::unique_ptr<Cubemap> diffuseCubemap_;
    std::unique_ptr<Cubemap> specularCubemap_;
private:
    int xResolution_;
    int yResolution_;
private:
    void SetupConstantBuffers();
    void SetupPreIntegratedData();
};
}