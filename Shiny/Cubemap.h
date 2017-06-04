#pragma once
#include "Platform.h"
#include <string>
namespace Shiny
{
class Cubemap
{
public:
    Cubemap(const std::string& name, bool enableMipmap = false, bool isInverted = false);
    ~Cubemap();
    void BindTextureUint(int unit);
    int GetMaxMipLevel() { return maxMipLevel_; }
    int GetWidth() { return width_; }
    int GetHeight() { return height_; }
private:
    GLuint textureID_ = 0;
    GLuint samplerID_ = 0;
    int width_ = 0;
    int height_ = 0;
    int maxMipLevel_ = 0;
};
}