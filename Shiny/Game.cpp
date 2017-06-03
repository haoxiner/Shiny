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
    //skyBoxShaderProgram_.Startup(ResourceManager::ReadFileToString("./Shaders/SkyBox.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/SkyBox.frag.glsl"));
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
    //glSamplerParameteri(samplerID_, GL_TEXTURE_MAX_LOD, 3);
    glBindSampler(0, samplerID_);
    glBindSampler(1, samplerID_);
    
    
    unsigned int w(0), h(0);
    FIBITMAP* dib = FreeImage_Load(FIF_EXR, "../../Resources/envmap/uffizi_diffuse.exr");
    auto colorType = FreeImage_GetColorType(dib);
    auto bpp = FreeImage_GetBPP(dib);
    w = FreeImage_GetWidth(dib);
    h = FreeImage_GetHeight(dib);
    std::cerr << w << "," << h << std::endl;
    auto bits = FreeImage_GetBits(dib);
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID_);
    glTextureStorage2D(textureID_, 1, GL_RGB16F, w, h);
    glTextureSubImage2D(textureID_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
    FreeImage_Unload(dib);
    glBindTextureUnit(0, textureID_);

    dib = FreeImage_Load(FIF_EXR, "../../Resources/dfg.exr");
    w = FreeImage_GetWidth(dib);
    h = FreeImage_GetHeight(dib);
    std::cerr << w << "," << h << std::endl;
    bits = FreeImage_GetBits(dib);
    glCreateTextures(GL_TEXTURE_2D, 1, &dfgTexture_);
    glTextureStorage2D(dfgTexture_, 1, GL_RGB16F, w, h);
    glTextureSubImage2D(dfgTexture_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
    FreeImage_Unload(dib);
    glBindTextureUnit(1, dfgTexture_);

    dib = FreeImage_Load(FIF_EXR, "../../Resources/envmap/uffizi_specular_0.exr");
    w = FreeImage_GetWidth(dib);
    h = FreeImage_GetHeight(dib);
    std::cerr << w << "," << h << std::endl;
    bits = FreeImage_GetBits(dib);
    auto maxLevel = 5;
    glCreateTextures(GL_TEXTURE_2D, 1, &specularEnvmapID_);
    glBindTextureUnit(2, specularEnvmapID_);
    glTextureStorage2D(specularEnvmapID_, maxLevel + 1, GL_RGB16F, w, h);
    for (int level = 0; level <= maxLevel; level++) {
        if (level > 0) {
            dib = FreeImage_Load(FIF_EXR, ("../../Resources/envmap/uffizi_specular_" + std::to_string(level) + ".exr").c_str());
            w = FreeImage_GetWidth(dib);
            h = FreeImage_GetHeight(dib);
            std::cerr << w << "," << h << std::endl;
            bits = FreeImage_GetBits(dib);
        }
        glTextureSubImage2D(specularEnvmapID_, level, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
        FreeImage_Unload(dib);
    }
    glCreateSamplers(1, &specularSamplerID_);
    glSamplerParameteri(specularSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(specularSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(specularSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(specularSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(specularSamplerID_, GL_TEXTURE_MAX_LOD, maxLevel);
    glSamplerParameteri(specularSamplerID_, GL_TEXTURE_MIN_LOD, 0);
    glBindSampler(2, specularSamplerID_);
    //cubemap
    {
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        const std::string FACE_NAME[] = { "PX", "NX", "PY", "NY", "PZ", "NZ" };
        for (int face = 0; face < 6; face++) {
            std::string fileName = "../../uffizi/uffizi-" + FACE_NAME[face] + ".exr";
            auto bitmap = FreeImage_Load(FIF_EXR, fileName.c_str());
            auto bpp = FreeImage_GetBPP(bitmap);
            std::cerr << "BPP: " << bpp << std::endl;
            auto width = FreeImage_GetWidth(bitmap);
            auto height = FreeImage_GetHeight(bitmap);
            auto bits = FreeImage_GetBits(bitmap);
            if (face == 0) {
                //srcMaxMipLevel_ = static_cast<int>(std::log2f(width));
                glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemapID_);
                glTextureStorage2D(cubemapID_, 1, GL_RGB16F, width, width);
            }
            FreeImage_FlipHorizontal(bitmap);
            FreeImage_FlipVertical(bitmap);
            glTextureSubImage3D(cubemapID_, 0, 0, 0, face, width, width, 1, (bpp == 96 ? GL_RGB : GL_RGBA), GL_FLOAT, bits);
            FreeImage_Unload(bitmap);
        }
        std::cerr << "finish" << std::endl;
        glBindTextureUnit(3, cubemapID_);
        glCreateSamplers(1, &cubemapSamplerID_);
        glSamplerParameteri(cubemapSamplerID_, GL_TEXTURE_CUBE_MAP_SEAMLESS, 1);
        glSamplerParameteri(cubemapSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(cubemapSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(cubemapSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(cubemapSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(cubemapSamplerID_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(cubemapSamplerID_, GL_TEXTURE_MAX_LOD, 0);
        glSamplerParameteri(cubemapSamplerID_, GL_TEXTURE_MIN_LOD, 0);
        glBindSampler(3, cubemapSamplerID_);
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
}

void Shiny::Game::Render()
{
    //testFloat_ = 5.0f;
    auto sinTheta = std::sinf(DegreesToRadians(testFloat_ * 2.0f));
    auto cosTheta = std::cosf(DegreesToRadians(testFloat_ * 2.0f));
    Quaternion quat(sinTheta, 0.0f, 0.0f, cosTheta);
    auto perFrameBuffer = static_cast<PerFrameConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], GL_WRITE_ONLY));
    perFrameBuffer->data = Float4(sinTheta * 0.5 + 0.5, cosTheta * 0.5 + 0.5, (sinTheta * 0.5 + cosTheta * 0.5) *0.5 + 0.5, 1.0);
    perFrameBuffer->worldToView = Matrix4x4(1.0f);
    glUnmapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram_.Use();
    PerObjectConstantBuffer perObjectBuffer;
    for (auto&& mesh : meshes_) {
        for (int i = 0; i <= 10; i++) {
            perObjectBuffer.modelToWorld = MakeTranslationMatrix(Float3(0, 0, -5)) * /*QuaternionToMatrix(Normalize(quat)) * */MakeTranslationMatrix(Float3(i * 2.2f - 11, 0, 0))*QuaternionToMatrix(Normalize(quat));// ;
            perObjectBuffer.material0 = Float4(1.0f - i / 10.0f, testMetallic_, 0.0f, 0.0f);
            glNamedBufferSubData(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], 0, sizeof(PerObjectConstantBuffer), &perObjectBuffer);
            mesh.Render();
        }
    }
}

void Shiny::Game::Shutdown()
{
    glDeleteBuffers(constantBufferList_.size(), constantBufferList_.data());
}
