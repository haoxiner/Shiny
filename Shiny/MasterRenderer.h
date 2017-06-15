#pragma once
#include "BatchOfStationaryEntity.h"
#include "BatchOfAnimatedEntity.h"
#include "Cubemap.h"
#include "ShaderProgram.h"
#include "SkyBox.h"
#include "Camera.h"
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
    void SetViewMatrix(const Matrix4x4& viewMatrix);
    void RenderSky(SkyBox& skyBox);
    void Update(float deltaTime);
    void Render(BatchOfStationaryEntity& batch);
    void Render(BatchOfAnimatedEntity& batch);
private:
    float deltaTime_ = 0.0f;
private:
    ShaderProgram stationaryEntityShader_;
    ShaderProgram skyBoxShader_;
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