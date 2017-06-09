#include "Material.h"
#include "Json.h"
#include "ResourceManager.h"
#include <FreeImage.h>
#include <iostream>

Shiny::Material::Material(const std::string& name)
{
    const std::string directory = "../../Resources/Material/" + name;

    Json::JsonObject config;
    Json::Parser parser(&config, ResourceManager::ReadFileToString(directory + "/material.json"));
    std::string type = config.GetValue("type").AsString();
    std::cerr << type << std::endl;

    const std::string dataList[] = { "basecolor.exr", "roughness.exr" };
    const GLenum dataTypeList[] = { GL_RGB16F, GL_R16F };
    for (int i = 0; i < 2; i++) {
        auto dib = FreeImage_Load(FIF_EXR, (directory + "/" + dataList[i]).c_str());
        auto w = FreeImage_GetWidth(dib);
        auto h = FreeImage_GetHeight(dib);
        auto bpp = FreeImage_GetBPP(dib);
        auto bits = FreeImage_GetBits(dib);
        std::cerr << "Material: " << w << "," << h << "," << bpp << std::endl;
        GLuint textureID;
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
        textureIDList_.push_back(textureID);
        glTextureStorage2D(textureID, 1, dataTypeList[i], w, h);
        GLenum components = GL_RGB;
        switch (bpp) {
        case 128:
            components = GL_RGBA;
            std::cerr << "RGBA" << std::endl;
            break;
        case 96:
            components = GL_RGB;
            std::cerr << "RGB" << std::endl;
            break;
        case 64:
            components = GL_RG;
            std::cerr << "RG" << std::endl;
            break;
        case 32:
            components = GL_RED;
            std::cerr << "RED" << std::endl;
            break;
        }
        glTextureSubImage2D(textureID, 0, 0, 0, w, h, components, GL_FLOAT, bits);
        FreeImage_Unload(dib);
    }
}

Shiny::Material::~Material()
{
    glDeleteTextures(textureIDList_.size(), textureIDList_.data());
}

void Shiny::Material::Use()
{
    for (int i = 0; i < textureIDList_.size(); i++) {
        glBindTextureUnit(3 + i, textureIDList_[i]);
    }
}
