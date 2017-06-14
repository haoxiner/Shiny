#pragma once
#include "Platform.h"
#include <string>
namespace Shiny
{
class Animation
{
public:
    Animation(const std::string& name);
    void Use();
private:
    GLuint animationFrameBufferID_;
};
}