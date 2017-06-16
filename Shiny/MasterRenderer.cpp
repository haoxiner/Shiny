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
    Float4 animationState;
};
struct PerFrameConstantBuffer
{
    Float4 eye;
    Matrix4x4 worldToView;
};
struct StaticConstantBuffer
{
    Matrix4x4 viewToProjectionForYup;
    Matrix4x4 viewToProjectionForZup;
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
    glDepthFunc(GL_LESS);
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

    stationaryEntityShader_.Startup(ResourceManager::ReadFileToString("./Shaders/Stationary.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));
    animatingEntityShader_.Startup(ResourceManager::ReadFileToString("./Shaders/PBR.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));
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
void MasterRenderer::SetCameraPose(const Matrix4x4& viewMatrix, const Float3& position)
{
    auto perFrameBuffer = static_cast<PerFrameConstantBuffer*>(glMapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER], GL_WRITE_ONLY));
    perFrameBuffer->eye.x = position.x;
    perFrameBuffer->eye.y = position.y;
    perFrameBuffer->eye.z = position.z;
    perFrameBuffer->worldToView = viewMatrix;
    glUnmapNamedBuffer(constantBufferList_[PER_FRAME_CONSTANT_BUFFER]);
}
void MasterRenderer::RenderSky(SkyBox& skyBox)
{
    skyBoxShader_.Use();
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_FRAMEBUFFER_SRGB);
    skyBox.Render();
    glDisable(GL_FRAMEBUFFER_SRGB);
    glDepthFunc(GL_LESS);
}
void MasterRenderer::Update(float deltaTime)
{
    deltaTime_ = deltaTime;
}
void MasterRenderer::Render(BatchOfStationaryEntity& batch)
{
    glEnable(GL_FRAMEBUFFER_SRGB);
    stationaryEntityShader_.Use();
    PerObjectConstantBuffer perObjectBuffer;
    glBindTextureUnit(0, dfgTextureID_);
    glBindSampler(0, defaultSamplerID_);
    diffuseCubemap_->BindTextureUint(1);
    specularCubemap_->BindTextureUint(2);
    perObjectBuffer.modelToWorld = MakeScaleMatrix(2,2,2);
    glNamedBufferSubData(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], 0, sizeof(PerObjectConstantBuffer), &perObjectBuffer);
    for (auto&& entity : batch.entityList_) {
        //perObjectBuffer.modelToWorld = MakeScaleMatrix(entity.scale_);// ;
        //glNamedBufferSubData(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], 0, sizeof(PerObjectConstantBuffer), &perObjectBuffer);
        for (auto&& pair : entity->models_) {
            auto material = pair.first;
            material->Use();
            for (auto&& mesh : pair.second) {
                mesh->Render();
            }
        }
    }
    glDisable(GL_FRAMEBUFFER_SRGB);
}
void MasterRenderer::Render(BatchOfAnimatingEntity& batch)
{
    glEnable(GL_FRAMEBUFFER_SRGB);

    static float animID = 0.0f;
    animID += deltaTime_ * 30;

    animatingEntityShader_.Use();
    PerObjectConstantBuffer perObjectBuffer;
    glBindTextureUnit(0, dfgTextureID_);
    glBindSampler(0, defaultSamplerID_);
    diffuseCubemap_->BindTextureUint(1);
    specularCubemap_->BindTextureUint(2);

    for (auto&& animEntityPair : batch.batch_) {
        auto& animation = animEntityPair.first;
        animation->Use();
        auto& entities = animEntityPair.second;
        for (auto&& entity : entities) {
            perObjectBuffer.animationState.x = (int(animID) % 21) * 3 * 60;
            perObjectBuffer.modelToWorld = MakeTranslationMatrix(entity->position_) * MakeScaleMatrix(entity->scale_) * QuaternionToMatrix(Normalize(entity->rotation_));// ;
            glNamedBufferSubData(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], 0, sizeof(PerObjectConstantBuffer), &perObjectBuffer);
            for (auto&& pair : entity->models_) {
                auto material = pair.first;
                material->Use();
                for (auto&& mesh : pair.second) {
                    mesh->Render();
                }
            }
        }
    }
    glDisable(GL_FRAMEBUFFER_SRGB);

}
void MasterRenderer::Render(const Terrain& terrain)
{
    glEnable(GL_FRAMEBUFFER_SRGB);
    stationaryEntityShader_.Use();
    PerObjectConstantBuffer perObjectBuffer;
    glBindTextureUnit(0, dfgTextureID_);
    glBindSampler(0, defaultSamplerID_);
    diffuseCubemap_->BindTextureUint(1);
    specularCubemap_->BindTextureUint(2);
    perObjectBuffer.modelToWorld = Matrix4x4(1.0);
    glNamedBufferSubData(constantBufferList_[PER_OBJECT_CONSTANT_BUFFER], 0, sizeof(PerObjectConstantBuffer), &perObjectBuffer);
    terrain.Render();
    glDisable(GL_FRAMEBUFFER_SRGB);
}
void MasterRenderer::SetupConstantBuffers()
{
    // GPU Resource
    constantBufferList_.resize(NUM_OF_CONSTANT_BUFFER);
    glCreateBuffers(constantBufferList_.size(), constantBufferList_.data());
    StaticConstantBuffer staticConstantBuffer;
    float sinTheta = std::sinf(DegreesToRadians(45));
    float cosTheta = std::cosf(DegreesToRadians(45));
    auto Rotate90degreeAboutXAxis = Matrix4x4(
        1, 0, 0, 0,
        0, 0, -1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1
    );
    staticConstantBuffer.viewToProjectionForYup = MakePerspectiveProjectionMatrix(45.0f, static_cast<float>(xResolution_) / yResolution_, 1.0f, 10000.0f);
    staticConstantBuffer.viewToProjectionForZup = staticConstantBuffer.viewToProjectionForYup/* * Rotate90degreeAboutXAxis*/;
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