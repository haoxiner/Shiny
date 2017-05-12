#pragma once
#include "Input.h"
#include "MathUtil.h"
namespace Shiny
{
class Camera {
public:
    const Vector3f& GetPosition() const;
    const Matrix4x4f& GetViewMatrix() const;
protected:
    Vector3f position_;
    float pitch_ = 0.0f;
    float yaw_ = 0.0f;
    float roll_ = 0.0f;
private:
    const float MAX_DISTANCE_FROM_PLAYER = 100.0f;
    float distanceFromPlayer_ = 100.0f;
    float angleAroundPlayer_ = 0.0f;
};
}