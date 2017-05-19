#pragma once
#include "Platform.h"
namespace Shiny
{
class VertexBuffer
{
public:
    GLuint GetID() const;
    void Render();
private:
    GLuint id_ = 0;
    int numOfIndex = 0;
};
}