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
    void SetEnvironment(const std::string& name);
    void IntegrateCubemap();
private:
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