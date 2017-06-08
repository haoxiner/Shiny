#include "Cubemap.h"
#include "Json.h"
#include "ResourceManager.h"
#include <FreeImage.h>
#include <fstream>
#include <iostream>

const std::string Shiny::Cubemap::FACE_NAME[6] = { "PX", "NX", "PY", "NY", "PZ", "NZ" };

Shiny::Cubemap::Cubemap(const std::string& name, const std::string& prefix, bool enableMipmap, bool isInversed)
{
    auto json = ResourceManager::ReadFileToString("../../output/" + prefix + "/" + name + ".json");
    Json::JsonObject config;
    Json::Parser parser(&config, json.c_str(), json.length());
    int configMaxMipLevel = 0;
    bool loadMipmapFromFile = false;
    if (!parser.HasError()) {
        configMaxMipLevel = config.GetValue("max_mip_level").AsInt();
        loadMipmapFromFile = config.GetValue("load_mipmap_from_file").AsBool();
    }
    maxMipLevel_ = configMaxMipLevel;
    std::cerr << "FILE MAX_MIP_LEVEL: " << configMaxMipLevel << std::endl;

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &textureID_);
    for (int face = 0; face < 6; face++) {
        std::string fileName = "../../output/" + prefix + "/" + name + "_" + FACE_NAME[face] + ".exr";
        auto bitmap = FreeImage_Load(FIF_EXR, fileName.c_str());
        auto bpp = FreeImage_GetBPP(bitmap);
        auto width = FreeImage_GetWidth(bitmap);
        auto height = FreeImage_GetHeight(bitmap);
        auto bits = FreeImage_GetBits(bitmap);
        if (face == 0) {
            width_ = width;
            height_ = height;
            std::cerr << width << "," << height << ", " << bpp << std::endl;
            if (enableMipmap && configMaxMipLevel == 0) {
                maxMipLevel_ = std::log2f(std::fminf(width_, height_));
            }
            std::cerr << "MAX_MIP_LEVEL: " << maxMipLevel_ << std::endl;
            glTextureStorage2D(textureID_, maxMipLevel_ + 1, GL_RGB16F, width, height);
        }
        if (!isInversed) {
            FreeImage_FlipHorizontal(bitmap);
            FreeImage_FlipVertical(bitmap);
        }
        glTextureSubImage3D(textureID_, 0, 0, 0, face, width, height, 1, (bpp == 96 ? GL_RGB : GL_RGBA), GL_FLOAT, bits);
        FreeImage_Unload(bitmap);
    }
    if (enableMipmap) {
        if (configMaxMipLevel == 0) {
            std::cerr << name << ": GEN" << std::endl;
            glGenerateTextureMipmap(textureID_);
        } else {
            std::cerr << name << ": LOAD" << std::endl;
            for (int face = 0; face < 6; face++) {
                for (int level = 1; level <= maxMipLevel_; level++) {
                    std::string fileName = "../../output/" + prefix + "/" + name + "_" + FACE_NAME[face] + "_" + std::to_string(level) + ".exr";
                    auto bitmap = FreeImage_Load(FIF_EXR, fileName.c_str());
                    auto bpp = FreeImage_GetBPP(bitmap);
                    auto width = FreeImage_GetWidth(bitmap);
                    auto height = FreeImage_GetHeight(bitmap);
                    auto bits = FreeImage_GetBits(bitmap);
                    if (!isInversed) {
                        FreeImage_FlipHorizontal(bitmap);
                        FreeImage_FlipVertical(bitmap);
                    }
                    glTextureSubImage3D(textureID_, level, 0, 0, face, width, width, 1, (bpp == 96 ? GL_RGB : GL_RGBA), GL_FLOAT, bits);
                    FreeImage_Unload(bitmap);
                }
            }
        }
    }
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCreateSamplers(1, &samplerID_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_CUBE_MAP_SEAMLESS, 1);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_FILTER, enableMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAX_LOD, maxMipLevel_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_LOD, 0);
}

Shiny::Cubemap::Cubemap(const std::string& directory, const std::string& fileID)
{
    auto json = ResourceManager::ReadFileToString(directory + "/" + fileID + ".json");
    Json::JsonObject config;
    Json::Parser parser(&config, json.c_str(), json.length());
    int configMaxMipLevel = 0;
    bool loadMipmapFromFile = false;
    if (!parser.HasError()) {
        configMaxMipLevel = config.GetValue("max_mip_level").AsInt();
        loadMipmapFromFile = config.GetValue("load_mipmap_from_file").AsBool();
    }
    maxMipLevel_ = configMaxMipLevel;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &textureID_);
    
    for (int face = 0; face < 6; face++) {
        std::string fileName = directory + "/" + fileID + "_" + FACE_NAME[face] + ".exr";
        auto bitmap = FreeImage_Load(FIF_EXR, fileName.c_str());
        auto bpp = FreeImage_GetBPP(bitmap);
        auto width = FreeImage_GetWidth(bitmap);
        auto height = FreeImage_GetHeight(bitmap);
        if (face == 0) {
            width_ = width;
            height_ = height;
            std::cerr << width << "," << height << ", " << bpp << std::endl;
            std::cerr << "MAX_MIP_LEVEL: " << maxMipLevel_ << std::endl;
            glTextureStorage2D(textureID_, maxMipLevel_ + 1, GL_RGB16F, width, height);
        }
        FreeImage_FlipHorizontal(bitmap);
        FreeImage_FlipVertical(bitmap);
        auto bits = FreeImage_GetBits(bitmap);
        glTextureSubImage3D(textureID_, 0, 0, 0, face, width, height, 1, GL_RGB, GL_FLOAT, bits);
        FreeImage_Unload(bitmap);
    }
    if (loadMipmapFromFile) {
        for (int face = 0; face < 6; face++) {
            for (int level = 1; level <= maxMipLevel_; level++) {
                std::string fileName = directory + "/" + fileID + "_" + FACE_NAME[face] + "_" + std::to_string(level) + ".exr";
                auto bitmap = FreeImage_Load(FIF_EXR, fileName.c_str());
                auto bpp = FreeImage_GetBPP(bitmap);
                auto width = FreeImage_GetWidth(bitmap);
                auto height = FreeImage_GetHeight(bitmap);
                FreeImage_FlipHorizontal(bitmap);
                FreeImage_FlipVertical(bitmap);
                auto bits = FreeImage_GetBits(bitmap);
                glTextureSubImage3D(textureID_, level, 0, 0, face, width, width, 1, GL_RGB, GL_FLOAT, bits);
                FreeImage_Unload(bitmap);
            }
        }
    } else {
        if (maxMipLevel_ > 0) {
            glGenerateTextureMipmap(textureID_);
            std::cerr << "GENERATE" << std::endl;
        }
    }
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCreateSamplers(1, &samplerID_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_CUBE_MAP_SEAMLESS, 1);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_FILTER, maxMipLevel_ > 0 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAX_LOD, maxMipLevel_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_LOD, 0);
}

Shiny::Cubemap::~Cubemap()
{
    glDeleteSamplers(1, &samplerID_);
    glDeleteTextures(1, &textureID_);
}

void Shiny::Cubemap::BindTextureUint(int unit) const
{
    glBindTextureUnit(unit, textureID_);
    glBindSampler(unit, samplerID_);
}
