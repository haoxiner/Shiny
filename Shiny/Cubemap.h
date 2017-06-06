#pragma once
#include "Platform.h"
#include <string>
namespace Shiny
{
class Cubemap
{
public:
    Cubemap(const std::string& name, bool enableMipmap = false, bool isInverted = false);
    Cubemap(const std::string& fileID, const std::string& directory);
    ~Cubemap();
    void BindTextureUint(int unit) const;
    int GetMaxMipLevel() const { return maxMipLevel_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    static const std::string FACE_NAME[6];
private:
    GLuint textureID_ = 0;
    GLuint samplerID_ = 0;
    int width_ = 0;
    int height_ = 0;
    int maxMipLevel_ = 0;
};
}