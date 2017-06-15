#pragma once
#include "MathUtil.h"
namespace Shiny
{
class ThirdPersonCamera
{
public:
    Matrix4x4 GetViewMatrix(const Float3& focusPosition) const;
    void AddForce(float horizontal, float vertical);
private:
    static const float DEFAULT_THETA;
    static const float DEFAULT_PHI;
    // horizontal
    float theta_ = DEFAULT_THETA;
    // vertical
    float phi_ = DEFAULT_PHI;
};
}