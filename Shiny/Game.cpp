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
    std::vector<float> fnormals = {
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };
    std::vector<unsigned int> indices = { 0,1,2, 0,2,3 };
    meshes_.emplace_back(2);
    auto&& mesh = meshes_.back();
    ResourceManager::LoadObjToMesh("../../cube.obj", mesh);
    //mesh.LoadVertexAttribute(0, 4, true, positions);
    //mesh.LoadVertexAttribute(1, 4, true, normals);
    //mesh.LoadVertexAttribute(1, 4, fnormals);
    //mesh.LoadIndices(indices);

    // Shader Program
    shaderProgram_.Startup(ResourceManager::ReadFileToString("./Shaders/PBR.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));
    //skyBoxShaderProgram_.Startup(ResourceManager::ReadFileToString("./Shaders/SkyBox.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/SkyBox.frag.glsl"));
    // GPU Resource
    constantBufferList_.resize(NUM_OF_CONSTANT_BUFFER);
    glCreateBuffers(constantBufferList_.size(), constantBufferList_.data());
    StaticConstantBuffer staticConstantBuffer;
    staticConstantBuffer.viewToProjection = MakePerspectiveProjectionMatrix(45.0f, static_cast<float>(xResolution) / yResolution, 0.001f, 1000.0f);
    glNamedBufferStorage(constantBufferList_[STATIC_CONSTANT_BUFFER], sizeof(StaticConstantBuffer), &staticConstantBuffer, 0);
    glNamedBufferStorage(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], sizeof(PerFrameConstantBuffer), nullptr, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], sizeof(PerObjectConstantBuffer), nullptr, GL_MAP_WRITE_BIT);
    
    for (int i = 0; i < constantBufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, constantBufferList_[i]);
    }
    
    glCreateSamplers(1, &samplerID_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindSampler(0, samplerID_);
    
    unsigned int w(0), h(0);
    FIBITMAP* dib = FreeImage_Load(FIF_EXR, "../../envmap.exr");
    auto colorType = FreeImage_GetColorType(dib);
    auto bpp = FreeImage_GetBPP(dib);
    w = FreeImage_GetWidth(dib);
    h = FreeImage_GetHeight(dib);
    auto bits = FreeImage_GetBits(dib);
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID_);
    glTextureStorage2D(textureID_, 1, GL_RGB16F, w, h);
    glTextureSubImage2D(textureID_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
    FreeImage_Unload(dib);
    glBindTextureUnit(0, textureID_);
    return true;
}

void Shiny::Game::Update(float deltaTime)
{
    testFloat_ += deltaTime;
}

void Shiny::Game::Render()
{
    auto sinTheta = std::sinf(DegreesToRadians(testFloat_ * 10.0f));
    auto cosTheta = std::cosf(DegreesToRadians(testFloat_ * 10.0f));

    auto perFrameBuffer = static_cast<PerFrameConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], GL_WRITE_ONLY));
    perFrameBuffer->data = Float4(sinTheta * 0.5 + 0.5, cosTheta * 0.5 + 0.5, (sinTheta * 0.5 + cosTheta * 0.5) *0.5 + 0.5, 1.0);
    perFrameBuffer->worldToView = Matrix4x4(1.0f);
    glUnmapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram_.Use();
    for (auto&& mesh : meshes_) {
        auto perObjectBuffer = static_cast<PerObjectConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], GL_WRITE_ONLY));
        Quaternion quat(0.0f, sinTheta, 0.0f, cosTheta);
        perObjectBuffer->modelToWorld = MakeTranslationMatrix(Float3(0.0f, 0.0f,-3.0f)) * QuaternionToMatrix(Normalize(quat));
        glUnmapNamedBuffer(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER]);
        mesh.Render();
    }
}

void Shiny::Game::Shutdown()
{
    glDeleteBuffers(constantBufferList_.size(), constantBufferList_.data());
}
