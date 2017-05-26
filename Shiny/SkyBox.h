#pragma once
#include "Mesh.h"
namespace Shiny
{
class SkyBox
{
public:
    SkyBox();
private:
    Mesh mesh_;
    GLuint textureID_;
};
}