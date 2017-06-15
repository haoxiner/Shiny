#pragma once
#include "MathUtil.h"
namespace Shiny
{
class Camera {
public:
    virtual Matrix4x4 GetViewMatrix() const = 0;
};
}