#include "Game.h"
#include "ResourceManager.h"
#include <fstream>
bool Shiny::Game::Startup(int xResolution, int yResolution, const Input* input)
{
    // OpenGL State
    renderingSystem_.EnableDepthTest();
    renderingSystem_.DisableCullFace();
    renderingSystem_.SetViewport(0, 0, xResolution, yResolution);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // Load Meshes
    std::vector<short> positions = {
        MapToShort(0.5f), MapToShort(0.5f), MapToShort(0.0f), MapToShort(1.0f),
        MapToShort(-0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(1.0f)
    };
    std::vector<short> normals = {
        MapToShort(0.0f), MapToShort(0.0f), MapToShort(1.0f), MapToShort(1.0f),
        MapToShort(0.0f), MapToShort(0.0f), MapToShort(1.0f), MapToShort(0.0f),
        MapToShort(0.0f), MapToShort(0.0f), MapToShort(1.0f), MapToShort(0.0f)
    };
    std::vector<unsigned int> indices = { 0,1,2 };
    meshes_.emplace_back(2);
    auto&& mesh = meshes_.back();
    mesh.LoadVertexAttribute(0, 4, true, positions);
    mesh.LoadVertexAttribute(1, 4, true, normals);
    mesh.LoadIndices(indices);

    // Shader Program
    shaderProgram_.Startup(ResourceManager::ReadFileToString("./Shaders/PBR.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));

    // GPU Resource
    constantBufferList_.resize(NUM_OF_CONSTANT_BUFFER);
    glCreateBuffers(constantBufferList_.size(), constantBufferList_.data());
    StaticConstantBuffer staticConstantBuffer;
    staticConstantBuffer.viewToProjection = MakePerspectiveProjectionMatrix(65.0f, static_cast<float>(xResolution) / yResolution, 0.001f, 1000.0f);
    glNamedBufferStorage(constantBufferList_[STATIC_CONSTANT_BUFFER], sizeof(StaticConstantBuffer), &staticConstantBuffer, 0);
    glNamedBufferStorage(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], sizeof(PerFrameConstantBuffer), nullptr, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], sizeof(PerObjectConstantBuffer), nullptr, GL_MAP_WRITE_BIT);
    
    for (int i = 0; i < constantBufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, constantBufferList_[i]);
    }
    return true;
}

void Shiny::Game::Update(float deltaTime)
{
    testFloat_ += deltaTime;
}

void Shiny::Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram_.Use();
    for (auto&& mesh : meshes_) {
        auto perFrameBuffer = static_cast<PerObjectConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], GL_WRITE_ONLY));
        auto sinTheta = std::sinf(DegreesToRadians(testFloat_ * 40.0f));
        auto cosTheta = std::cosf(DegreesToRadians(testFloat_ * 40.0f));
        Quaternion quat(0.0f, 0.0f, sinTheta, cosTheta);
        perFrameBuffer->modelToWorld = MakeTranslationMatrix(Float3(0.0f, 0.0f,sinTheta * 0.8f - 0.9f)) * QuaternionToMatrix(quat);
        perFrameBuffer->worldToView = Matrix4x4(1.0f);
        glUnmapNamedBuffer(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER]);
        mesh.Render();
    }
}

void Shiny::Game::Shutdown()
{
    glDeleteBuffers(constantBufferList_.size(), constantBufferList_.data());
}
