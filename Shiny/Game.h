#pragma once
#include "RenderingSystem.h"
#include "Input.h"
#include "Mesh.h"
#include "Material.h"
#include "Entity.h"
#include "ShaderProgram.h"
#include "MathUtil.h"
#include <vector>
namespace Shiny
{
class Game
{
public:
    bool Startup(int xResolution, int yResolution, const Input* input);
    void Update(float deltaTime);
    void Render();
    void Shutdown();
private:
    RenderingSystem renderingSystem_;
    ShaderProgram shaderProgram_;
    ShaderProgram skyBoxShaderProgram_;
    std::vector<Mesh> meshes_;

    struct PerObjectConstantBuffer
    {
        Matrix4x4 modelToWorld;
    };
    struct PerFrameConstantBuffer
    {
        Float4 data;
        Matrix4x4 worldToView;
    };
    struct StaticConstantBuffer
    {
        Matrix4x4 viewToProjection;
    };
    enum ConstantBufferType
    {
        STATIC_CONSTANT_BUFFER = 0,
        PER_FRAME_CONSTANT_BUFFER,
        PER_OBJECT_CONSTANT_BUFFER,
        NUM_OF_CONSTANT_BUFFER
    };
    std::vector<GLuint> constantBufferList_;
    GLuint textureID_;
    GLuint specularEnvmapID_;
    GLuint dfgTexture_;
    GLuint samplerID_;
    GLuint cubemapID_;
    float testFloat_ = 0.0f;
};
}