#include "MasterRenderer.h"
#include <FreeImage.h>
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

    glCreateSamplers(1, &defaultSamplerID_);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glCreateSamplers(1, &repeatSamplerID_);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return true;
}
void MasterRenderer::Shutdown()
{
    glDeleteTextures(1, &dfgTextureID_);
    glDeleteSamplers(1, &defaultSamplerID_);
    glDeleteSamplers(1, &repeatSamplerID_);
    glDeleteBuffers(constantBufferList_.size(), constantBufferList_.data());
}
void MasterRenderer::SetEnvironment(const std::string& name)
{
    diffuseCubemap_.reset(new Cubemap("../../Resources/Environment", name + "_diffuse"));
    diffuseCubemap_->BindTextureUint(1);
    specularCubemap_.reset(new Cubemap("../../Resources/Environment", name + "_specular"));
    specularCubemap_->BindTextureUint(2);
}
void MasterRenderer::IntegrateCubemap()
{
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
}
}