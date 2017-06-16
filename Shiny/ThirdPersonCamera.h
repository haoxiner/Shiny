#pragma once
#include "MathUtil.h"
#include "Input.h"
namespace Shiny
{
class ThirdPersonCamera
{
public:
    void Update(float deltaTime, const Input& input, float forwardRotation);
    void GetPose(Matrix4x4& viewMatrix, Float3& cameraPosition, const Float3& focusPosition);
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

