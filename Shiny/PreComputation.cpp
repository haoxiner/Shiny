#include "PreComputation.h"
#include "Display.h"
#include "ResourceManager.h"
#include "FreeImage.h"
#include <algorithm>
#include <iostream>

Shiny::PreComputation::PreComputation()
{
    shaderSource_ = Shiny::ResourceManager::ReadFileToString("./Shaders/PreComputation.comp.glsl");
    glCreateBuffers(1, &inputBufferID_);
    glNamedBufferStorage(inputBufferID_, sizeof(InputBlock), nullptr, GL_MAP_WRITE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, inputBufferID_);
}

Shiny::PreComputation::~PreComputation()
{
    delete cubemap_;
}

void Shiny::PreComputation::LoadSourceFile(const std::string& fileName)
{
    //FIBITMAP* dib = FreeImage_Load(FIF_EXR, fileName.c_str());
    //auto w = FreeImage_GetWidth(dib);
    //auto h = FreeImage_GetHeight(dib);
    //auto bits = FreeImage_GetBits(dib);
    //srcMaxLevel_ = static_cast<int>(std::log2f(std::fminf(w, h)));
    //glCreateTextures(GL_TEXTURE_2D, 1, &srcTextureID_);
    //glTextureStorage2D(srcTextureID_, srcMaxLevel_ + 1, GL_RGB32F, w, h);
    //glTextureSubImage2D(srcTextureID_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
    //FreeImage_Unload(dib);
    //glGenerateTextureMipmap(srcTextureID_);
    //glBindTextureUnit(0, srcTextureID_);
    //glCreateSamplers(1, &linearSamplerID_);
    //glSamplerParameteri(linearSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glSamplerParameteri(linearSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glSamplerParameteri(linearSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glSamplerParameteri(linearSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glSamplerParameteri(linearSamplerID_, GL_TEXTURE_MAX_LOD, srcMaxLevel_);
    //glSamplerParameteri(linearSamplerID_, GL_TEXTURE_MIN_LOD, 0);
    //glBindSampler(0, linearSamplerID_);

    cubemap_ = new Cubemap(fileName, true);
    srcMaxLevel_ = cubemap_->GetMaxMipLevel();
    cubemap_->BindTextureUint(0);
}

void Shiny::PreComputation::ComputeIBLDFG(const std::string& destFileName)
{
    int w = 128, h = 128;
    auto inputBuffer = reinterpret_cast<InputBlock*>(glMapNamedBuffer(inputBufferID_, GL_WRITE_ONLY));
    inputBuffer->inputArg0 = { w,h,2,1024 * 64 };
    glUnmapNamedBuffer(inputBufferID_);
    glDeleteTextures(1, &destTextureID_);
    glCreateTextures(GL_TEXTURE_2D, 1, &destTextureID_);
    glBindImageTexture(0, destTextureID_, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glTextureStorage2D(destTextureID_, 1, GL_RGBA32F, w, h);
    ShaderProgram computeShaderProgram;
    int local_size_x = 32;
    int local_size_y = 32;
    int local_size_z = 1;
    computeShaderProgram.Startup(shaderSource_, local_size_x, local_size_y, local_size_z);
    computeShaderProgram.Use();
    glDispatchCompute(w / local_size_x, h / local_size_y, local_size_z);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    computeShaderProgram.Shutdown();
    Save(destFileName, w, h);
}

void Shiny::PreComputation::ComputeIBLDiffuse(const std::string& destFileName)
{
    const std::string FACE_NAME[] = { "PX", "NX", "PY", "NY", "PZ", "NZ" };
    int w = 64;
    int h = w;
    int sampleCount = 1024 * 4;
    for (int face = 0; face < 6; face++) {
        auto inputBuffer = reinterpret_cast<InputBlock*>(glMapNamedBuffer(inputBufferID_, GL_WRITE_ONLY));
        inputBuffer->inputArg0 = { w,h,0, sampleCount };
        inputBuffer->inputArg1.x = srcMaxLevel_;
        inputBuffer->inputArg1.y = 0;
        inputBuffer->inputArg1.z = 0;
        inputBuffer->inputArg1.w = face;
        glUnmapNamedBuffer(inputBufferID_);
        glDeleteTextures(1, &destTextureID_);
        glCreateTextures(GL_TEXTURE_2D, 1, &destTextureID_);
        glBindImageTexture(0, destTextureID_, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glTextureStorage2D(destTextureID_, 1, GL_RGBA32F, w, h);
        ShaderProgram computeShaderProgram;
        int local_size_x = std::min(h, 32);
        int local_size_y = local_size_x;
        int local_size_z = 1;
        computeShaderProgram.Startup(shaderSource_, local_size_x, local_size_y, local_size_z);
        computeShaderProgram.Use();
        glDispatchCompute(w / local_size_x, h / local_size_y, local_size_z);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        computeShaderProgram.Shutdown();
        Save(destFileName + "_" + FACE_NAME[face] + ".exr", w, h, true, true);
    }
    std::cerr << w << "," << h << std::endl;
    //auto inputBuffer = reinterpret_cast<InputBlock*>(glMapNamedBuffer(inputBufferID_, GL_WRITE_ONLY));
    //inputBuffer->inputArg0 = { w,h,0,1024 * 64 };
    //glUnmapNamedBuffer(inputBufferID_);
    //glDeleteTextures(1, &destTextureID_);
    //glCreateTextures(GL_TEXTURE_2D, 1, &destTextureID_);
    //glBindImageTexture(0, destTextureID_, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    //glTextureStorage2D(destTextureID_, 1, GL_RGBA32F, w, h);
    //ShaderProgram computeShaderProgram;
    //int local_size_x = 32;
    //int local_size_y = 32;
    //int local_size_z = 1;
    //computeShaderProgram.Startup(shaderSource_, local_size_x, local_size_y, local_size_z);
    //computeShaderProgram.Use();
    //glDispatchCompute(w / local_size_x, h / local_size_y, local_size_z);
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //computeShaderProgram.Shutdown();
    //Save(destFileName, w, h);
}

void Shiny::PreComputation::ComputeIBLSpecular(const std::string& destFileName)
{
    const std::string FACE_NAME[] = { "PX", "NX", "PY", "NY", "PZ", "NZ" };
    int w = 512;
    int h = w;
    int sampleCount = 1024 * 4;
    int maxLevel = 5;
    for (int level = 0; level <= maxLevel; level++) {
        ShaderProgram computeShaderProgram;
        int local_size_x = std::min(h, 32);
        int local_size_y = local_size_x;
        int local_size_z = 1;
        computeShaderProgram.Startup(shaderSource_, local_size_x, local_size_y, local_size_z);
        computeShaderProgram.Use();
        for (int face = 0; face < 6; face++) {
            auto inputBuffer = reinterpret_cast<InputBlock*>(glMapNamedBuffer(inputBufferID_, GL_WRITE_ONLY));
            inputBuffer->inputArg0 = { w,h,1, sampleCount };
            inputBuffer->inputArg1.x = srcMaxLevel_;
            inputBuffer->inputArg1.y = level;
            inputBuffer->inputArg1.z = maxLevel;
            inputBuffer->inputArg1.w = face;
            glUnmapNamedBuffer(inputBufferID_);
            glDeleteTextures(1, &destTextureID_);
            glCreateTextures(GL_TEXTURE_2D, 1, &destTextureID_);
            glBindImageTexture(0, destTextureID_, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
            glTextureStorage2D(destTextureID_, 1, GL_RGBA32F, w, h);

            glDispatchCompute(w / local_size_x, h / local_size_y, local_size_z);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            
            Save(destFileName + "_" + FACE_NAME[face] + (level > 0 ? ("_" + std::to_string(level)) : "") + ".exr", w, h, true, true);
        }
        computeShaderProgram.Shutdown();
        std::cerr << w << "," << h << std::endl;
        w = w / 2;
        h = h / 2;
    }
}

void Shiny::PreComputation::Save(const std::string& fileName, int width, int height, bool invertHorizontal, bool invertVertical)
{
    auto texSize = sizeof(float) * 3 * width * height;
    auto bitmap = FreeImage_AllocateT(FIT_RGBF, width, height);
    auto pixels = FreeImage_GetBits(bitmap);
    glGetTextureImage(destTextureID_, 0, GL_RGB, GL_FLOAT, texSize, pixels);
    if (invertHorizontal) {
        FreeImage_FlipHorizontal(bitmap);
    }
    if (invertVertical) {
        FreeImage_FlipVertical(bitmap);
    }
    FreeImage_Save(FIF_EXR, bitmap, fileName.c_str(), EXR_DEFAULT);
    FreeImage_Unload(bitmap);
}
