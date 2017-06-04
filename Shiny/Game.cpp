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
    ResourceManager::LoadObjToMesh("../../Resources/Model/sphere.obj", mesh);
    //mesh.LoadVertexAttribute(0, 4, true, positions);
    //mesh.LoadVertexAttribute(1, 4, true, normals);
    //mesh.LoadIndices(indices);

    // Shader Program
    shaderProgram_.Startup(ResourceManager::ReadFileToString("./Shaders/PBR.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));
    // GPU Resource
    constantBufferList_.resize(NUM_OF_CONSTANT_BUFFER);
    glCreateBuffers(constantBufferList_.size(), constantBufferList_.data());
    StaticConstantBuffer staticConstantBuffer;
    staticConstantBuffer.viewToProjection = MakePerspectiveProjectionMatrix(45.0f, static_cast<float>(xResolution) / yResolution, 0.001f, 1000.0f);
    glNamedBufferStorage(constantBufferList_[STATIC_CONSTANT_BUFFER], sizeof(StaticConstantBuffer), &staticConstantBuffer, 0);
    glNamedBufferStorage(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], sizeof(PerFrameConstantBuffer), nullptr, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], sizeof(PerObjectConstantBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT);
    
    for (int i = 0; i < constantBufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, constantBufferList_[i]);
    }
    
    glCreateSamplers(1, &samplerID_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindSampler(1, samplerID_);

    glCreateSamplers(1, &materialSamplerID_);
    glSamplerParameteri(materialSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(materialSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(materialSamplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(materialSamplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindSampler(3, materialSamplerID_);
    glBindSampler(4, materialSamplerID_);
    glBindSampler(5, materialSamplerID_);
    
    {
        auto dib = FreeImage_Load(FIF_EXR, "../../Resources/dfg.exr");
        auto w = FreeImage_GetWidth(dib);
        auto h = FreeImage_GetHeight(dib);
        std::cerr << w << "," << h << std::endl;
        auto bits = FreeImage_GetBits(dib);
        glCreateTextures(GL_TEXTURE_2D, 1, &dfgTexture_);
        glTextureStorage2D(dfgTexture_, 1, GL_RGB16F, w, h);
        glTextureSubImage2D(dfgTexture_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
        FreeImage_Unload(dib);
        glBindTextureUnit(1, dfgTexture_);
    }
    {
        auto dib = FreeImage_Load(FIF_PNG, "../../Resources/Texture/octostone/albedo.png");
        //FreeImage_AdjustGamma(dib, 2.2);
        auto w = FreeImage_GetWidth(dib);
        auto h = FreeImage_GetHeight(dib);
        auto bpp = FreeImage_GetBPP(dib);
        std::cerr << w << "," << h <<  ", bpp: " << bpp << std::endl;
        auto bits = FreeImage_GetBits(dib);
        glCreateTextures(GL_TEXTURE_2D, 1, &baseColorMapID_);
        glTextureStorage2D(baseColorMapID_, 1, GL_RGB8, w, h);
        glTextureSubImage2D(baseColorMapID_, 0, 0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, bits);
        FreeImage_Unload(dib);
        glBindTextureUnit(3, baseColorMapID_);
    }
    {
        auto dib = FreeImage_Load(FIF_PNG, "../../Resources/Texture/octostone/roughness.png");
        auto w = FreeImage_GetWidth(dib);
        auto h = FreeImage_GetHeight(dib);
        auto bpp = FreeImage_GetBPP(dib);
        std::cerr << w << "," << h << ", bpp: " << bpp << std::endl;
        auto bits = FreeImage_GetBits(dib);
        glCreateTextures(GL_TEXTURE_2D, 1, &smoothnessMapID_);
        glTextureStorage2D(smoothnessMapID_, 1, GL_R8, w, h);
        glTextureSubImage2D(smoothnessMapID_, 0, 0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, bits);
        FreeImage_Unload(dib);
        glBindTextureUnit(4, smoothnessMapID_);
    }
    {
        auto dib = FreeImage_Load(FIF_PNG, "../../Resources/Texture/greasy_metal/metallic.png");
        auto w = FreeImage_GetWidth(dib);
        auto h = FreeImage_GetHeight(dib);
        auto bpp = FreeImage_GetBPP(dib);
        std::cerr << w << "," << h << ", bpp: " << bpp << std::endl;
        auto bits = FreeImage_GetBits(dib);
        glCreateTextures(GL_TEXTURE_2D, 1, &metallicMapID_);
        glTextureStorage2D(metallicMapID_, 1, GL_R8, w, h);
        glTextureSubImage2D(metallicMapID_, 0, 0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, bits);
        FreeImage_Unload(dib);
        glBindTextureUnit(5, metallicMapID_);
    }
    //cubemap
    {
        specularCubemap_ = new Cubemap("doge2_specular", true);
        specularCubemap_->BindTextureUint(2);
        diffuseCubemap_ = new Cubemap("doge2_diffuse", false);
        diffuseCubemap_->BindTextureUint(0);
    }
    return true;
}

void Shiny::Game::Update(float deltaTime, const Input* input)
{
    testFloat_ += deltaTime;
    if (input->Fire0()) {
        testMetallic_ = 0.0f;
    } else if (input->Test()) {
        testMetallic_ = 1.0f;
    }
    if (input->Jump()) {
        testDominant_ = 1.0f;
    } else {
        testDominant_ = 0.0f;
    }
}

void Shiny::Game::Render()
{
    //testFloat_ = 0;
    auto sinTheta = std::sinf(DegreesToRadians(testFloat_ * 6.0f));
    auto cosTheta = std::cosf(DegreesToRadians(testFloat_ * 6.0f));
    Quaternion quat(0.0f, sinTheta, 0.0f, cosTheta);
    auto perFrameBuffer = static_cast<PerFrameConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], GL_WRITE_ONLY));
    perFrameBuffer->data = Float4(sinTheta * 0.5 + 0.5, cosTheta * 0.5 + 0.5, (sinTheta * 0.5 + cosTheta * 0.5) *0.5 + 0.5, 1.0);
    perFrameBuffer->worldToView = Matrix4x4(1.0f);
    glUnmapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram_.Use();
    PerObjectConstantBuffer perObjectBuffer;
    for (auto&& mesh : meshes_) {
        for (int i = 0; i <= 0; i++) {
            auto smoothness = 1.0f - i / 10.0f;
            //perObjectBuffer.modelToWorld = MakeTranslationMatrix(Float3(0, 0, -15)) * MakeTranslationMatrix(Float3(i * 2.2f - 11, 0, 0)) * QuaternionToMatrix(Normalize(quat));// ;
            perObjectBuffer.modelToWorld = MakeTranslationMatrix(Float3(0,-0, -2)) * MakeScaleMatrix(1.0, 1.0, 1.0) * QuaternionToMatrix(Normalize(quat));// ;
            perObjectBuffer.material0 = Float4(smoothness, testMetallic_, testDominant_, 0.0f);
            glNamedBufferSubData(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], 0, sizeof(PerObjectConstantBuffer), &perObjectBuffer);
            mesh.Render();
        }
    }
}

void Shiny::Game::Shutdown()
{
    glDeleteBuffers(constantBufferList_.size(), constantBufferList_.data());
    delete specularCubemap_;
    delete diffuseCubemap_;
}
