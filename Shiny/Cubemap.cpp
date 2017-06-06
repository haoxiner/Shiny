#include "Cubemap.h"
#include <FreeImage.h>
#include <fstream>
#include <iostream>

const std::string Shiny::Cubemap::FACE_NAME[6] = { "PX", "NX", "PY", "NY", "PZ", "NZ" };

Shiny::Cubemap::Cubemap(const std::string& name, bool enableMipmap, bool isInversed)
{
    std::ifstream config("../../Resources/Environment/" + name + "/" + name + ".json");
    int fileMaxMipLevel = 0;
    if (config) {
        config >> fileMaxMipLevel;
        config.close();
    }
    maxMipLevel_ = fileMaxMipLevel;
    std::cerr << "FILE MAX_MIP_LEVEL: " << fileMaxMipLevel << std::endl;

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &textureID_);
    for (int face = 0; face < 6; face++) {
        std::string fileName = "../../Resources/Environment/" + name + "/" + name + "_" + FACE_NAME[face] + ".exr";
        auto bitmap = FreeImage_Load(FIF_EXR, fileName.c_str());
        auto bpp = FreeImage_GetBPP(bitmap);
        auto width = FreeImage_GetWidth(bitmap);
        auto height = FreeImage_GetHeight(bitmap);
        auto bits = FreeImage_GetBits(bitmap);
        if (face == 0) {
            width_ = width;
            height_ = height;
            std::cerr << width << "," << height << ", " << bpp << std::endl;
            if (enableMipmap && fileMaxMipLevel == 0) {
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
        if (fileMaxMipLevel == 0) {
            std::cerr << name << ": GEN" << std::endl;
            glGenerateTextureMipmap(textureID_);
        } else {
            std::cerr << name << ": LOAD" << std::endl;
            for (int face = 0; face < 6; face++) {
                for (int level = 1; level <= maxMipLevel_; level++) {
                    std::string fileName = "../../Resources/Environment/" + name + "/" + name + "_" + FACE_NAME[face] + "_" + std::to_string(level) + ".exr";
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

Shiny::Cubemap::Cubemap(const std::string& fileID, const std::string& directory)
{
    std::ifstream config(directory + "/" + fileID + ".json");
    if (config) {
        config >> maxMipLevel_;
        config.close();
        std::cerr << "FILE MAX_MIP_LEVEL: " << maxMipLevel_ << std::endl;
    }
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &textureID_);
    
    for (int face = 0; face < 6; face++) {
        std::string fileName = directory + "/" + fileID + "_" + FACE_NAME[face] + ".exr";
        auto bitmap = FreeImage_Load(FIF_EXR, fileName.c_str());
        auto bpp = FreeImage_GetBPP(bitmap);
        auto width = FreeImage_GetWidth(bitmap);
        auto height = FreeImage_GetHeight(bitmap);
        auto bits = FreeImage_GetBits(bitmap);
        if (face == 0) {
            width_ = width;
            height_ = height;
            std::cerr << width << "," << height << ", " << bpp << std::endl;
            std::cerr << "MAX_MIP_LEVEL: " << maxMipLevel_ << std::endl;
            glTextureStorage2D(textureID_, maxMipLevel_ + 1, GL_RGB16F, width, height);
        }
        glTextureSubImage3D(textureID_, 0, 0, 0, face, width, height, 1, (bpp == 96 ? GL_RGB : GL_RGBA), GL_FLOAT, bits);
        FreeImage_Unload(bitmap);
    }
    bool isInversed = false;
    bool enableMipmap = false;
    for (int face = 0; face < 6; face++) {
        for (int level = 1; level <= maxMipLevel_; level++) {
            std::string fileName = directory + "/" + fileID + "_" + FACE_NAME[face] + "_" + std::to_string(level) + ".exr";
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
