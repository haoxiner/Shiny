#pragma once
#include "MathUtil.h"
namespace Shiny
{
class CameraController
{
public:
    virtual Matrix4x4 GetViewMatrix() = 0;
};
}