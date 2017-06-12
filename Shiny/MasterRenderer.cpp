#include "MasterRenderer.h"
#include "ResourceManager.h"
#include <FreeImage.h>
#include <iostream>
namespace Shiny
{
struct PerObjectConstantBuffer
{
    Matrix4x4 modelToWorld;
    Float4 material0;
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
bool MasterRenderer::Startup(int xResolution, int yResolution)
{
    xResolution_ = xResolution;
    yResolution_ = yResolution;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glViewport(0, 0, xResolution, yResolution);

    glCreateSamplers(1, &defaultSamplerID_);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glCreateSamplers(1, &repeatSamplerID_);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindSampler(3, repeatSamplerID_);
    glBindSampler(4, repeatSamplerID_);
    glBindSampler(5, repeatSamplerID_);

    SetupConstantBuffers();
    SetupPreIntegratedData();
    SetupEnvironment("uffizi");

    stationaryEntityShader_.Startup(ResourceManager::ReadFileToString("./Shaders/PBR.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));
    skyBoxShader_.Startup(ResourceManager::ReadFileToString("./Shaders/SkyBox.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/SkyBox.frag.glsl"));
    return true;
}
void MasterRenderer::Shutdown()
{
    glDeleteTextures(1, &dfgTextureID_);
    glDeleteSamplers(1, &defaultSamplerID_);
    glDeleteSamplers(1, &repeatSamplerID_);
    glDeleteBuffers(constantBufferList_.size(), constantBufferList_.data());
}
void MasterRenderer::SetupEnvironment(const std::string& name)
{
    diffuseCubemap_.reset(new Cubemap("../../Resources/Environment/" + name, name + "_diffuse"));
    diffuseCubemap_->BindTextureUint(1);
    specularCubemap_.reset(new Cubemap("../../Resources/Environment/" + name, name + "_specular"));
    specularCubemap_->BindTextureUint(2);
}
void MasterRenderer::RenderSky(SkyBox& skyBox)
{
    skyBoxShader_.Use();
    skyBox.Render();
}
void MasterRenderer::Update(float deltaTime)
{
    static auto testFloat = 0.0f;
    deltaTime_ = deltaTime;
    testFloat += deltaTime_;
    //testFloat_ = 0;
    auto sinTheta = std::sinf(DegreesToRadians(testFloat * 6.0f));
    auto cosTheta = std::cosf(DegreesToRadians(testFloat * 6.0f));
    Quaternion quat(0.0f, sinTheta, 0.0f, cosTheta);
    auto perFrameBuffer = static_cast<PerFrameConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], GL_WRITE_ONLY));
    perFrameBuffer->data = Float4(sinTheta * 0.5 + 0.5, cosTheta * 0.5 + 0.5, (sinTheta * 0.5 + cosTheta * 0.5) *0.5 + 0.5, 1.0);
    perFrameBuffer->worldToView = Matrix4x4(1.0f)*QuaternionToMatrix(Normalize(quat));
    glUnmapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER]);
}
void MasterRenderer::Render(BatchOfStationaryEntity& batch)
{   
    stationaryEntityShader_.Use();
    PerObjectConstantBuffer perObjectBuffer;
    glBindTextureUnit(0, dfgTextureID_);
    glBindSampler(0, defaultSamplerID_);
    diffuseCubemap_->BindTextureUint(1);
    specularCubemap_->BindTextureUint(2);
    for (auto&& entity : batch.entityList_) {
        perObjectBuffer.modelToWorld = MakeTranslationMatrix(entity.position_) * MakeScaleMatrix(entity.scale_);// *QuaternionToMatrix(Normalize(quat));// ;
        glNamedBufferSubData(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], 0, sizeof(PerObjectConstantBuffer), &perObjectBuffer);
        for (auto&& pair : entity.models_) {
            auto material = pair.first;
            
            material->Use();
            for (auto&& mesh : pair.second) {
                mesh->Render();
            }
        }
    }
}
void MasterRenderer::SetupConstantBuffers()
{
    // GPU Resource
    constantBufferList_.resize(NUM_OF_CONSTANT_BUFFER);
    glCreateBuffers(constantBufferList_.size(), constantBufferList_.data());
    StaticConstantBuffer staticConstantBuffer;
    staticConstantBuffer.viewToProjection = MakePerspectiveProjectionMatrix(45.0f, static_cast<float>(xResolution_) / yResolution_, 0.001f, 1000.0f);
    glNamedBufferStorage(constantBufferList_[STATIC_CONSTANT_BUFFER], sizeof(StaticConstantBuffer), &staticConstantBuffer, 0);
    glNamedBufferStorage(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], sizeof(PerFrameConstantBuffer), nullptr, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], sizeof(PerObjectConstantBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT);
    for (int i = 0; i < constantBufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, constantBufferList_[i]);
    }
}
void MasterRenderer::SetupPreIntegratedData()
{
    // load dfg
    auto dib = FreeImage_Load(FIF_EXR, "../../Resources/Environment/dfg.exr");
    auto w = FreeImage_GetWidth(dib);
    auto h = FreeImage_GetHeight(dib);
    auto bits = FreeImage_GetBits(dib);
    glCreateTextures(GL_TEXTURE_2D, 1, &dfgTextureID_);
    glTextureStorage2D(dfgTextureID_, 1, GL_RGB16F, w, h);
    glTextureSubImage2D(dfgTextureID_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
    FreeImage_Unload(dib);
    glBindTextureUnit(0, dfgTextureID_);
    glBindSampler(0, defaultSamplerID_);
    std::cerr << "DFG: " << w << "," << h << std::endl;
}
}