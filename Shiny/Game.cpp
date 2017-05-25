#include "Game.h"
#include "ResourceManager.h"
#include "FreeImage.h"
#include <fstream>
#include <iostream>
bool Shiny::Game::Startup(int xResolution, int yResolution, const Input* input)
{
    // OpenGL State
    renderingSystem_.EnableDepthTest();
    renderingSystem_.DisableCullFace();
    renderingSystem_.SetViewport(0, 0, xResolution, yResolution);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // Load Meshes
    std::vector<short> positions = {
        MapToShort(1.0f), MapToShort(1.0f), MapToShort(0.0f), MapToShort(1.0f),
        MapToShort(-1.0f), MapToShort(1.0f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(-1.0f), MapToShort(-1.0f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(1.0f), MapToShort(-1.0f), MapToShort(0.0f), MapToShort(1.0f)
    };
    std::vector<short> normals = {
        MapToShort(0.0f), MapToShort(0.0f), MapToShort(1.0f), MapToShort(1.0f),
        MapToShort(0.0f), MapToShort(0.0f), MapToShort(1.0f), MapToShort(1.0f),
        MapToShort(0.0f), MapToShort(0.0f), MapToShort(1.0f), MapToShort(0.0f),
        MapToShort(0.0f), MapToShort(0.0f), MapToShort(1.0f), MapToShort(0.0f)
    };
    std::vector<unsigned int> indices = { 0,1,2, 0,2,3 };
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

    glGenTextures(1, &textureID_);
    unsigned int w(0), h(0);
    FIBITMAP* dib = FreeImage_Load(FIF_EXR, "G:\\haoxin\\mitsuba-af602c6fd98a\\data\\tests\\envmap.exr");
    //BYTE* bits = FreeImage_GetBits(dib);
    w = FreeImage_GetWidth(dib);
    h = FreeImage_GetHeight(dib);
    FreeImage_Unload(dib);
    std::cerr << w << "," << h << std::endl;
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
        auto perObjectBuffer = static_cast<PerObjectConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], GL_WRITE_ONLY));
        auto sinTheta = 0.0 * std::sinf(DegreesToRadians(testFloat_ * 40.0f));
        auto cosTheta = 1.0 + 0.0 * std::cosf(DegreesToRadians(testFloat_ * 40.0f));
        Quaternion quat(0.0f, 0.0f, sinTheta, cosTheta);
        perObjectBuffer->modelToWorld = MakeTranslationMatrix(Float3(0.0f, 0.0f,sinTheta * 0.8f - 0.9f)) * QuaternionToMatrix(quat);
        glUnmapNamedBuffer(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER]);

        auto perFrameBuffer = static_cast<PerFrameConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], GL_WRITE_ONLY));
        perFrameBuffer->data = Float4(sinTheta * 0.5 + 0.5, cosTheta * 0.5 + 0.5, (sinTheta * 0.5 + cosTheta * 0.5) *0.5 + 0.5, 1.0);
        perFrameBuffer->worldToView = Matrix4x4(1.0f);
        glUnmapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER]);
        mesh.Render();
    }
}

void Shiny::Game::Shutdown()
{
    glDeleteBuffers(constantBufferList_.size(), constantBufferList_.data());
}
