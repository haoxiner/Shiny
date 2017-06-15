#include "ThirdPersonCamera.h"
const float Shiny::ThirdPersonCamera::DEFAULT_THETA = 90.0f;
const float Shiny::ThirdPersonCamera::DEFAULT_PHI = 90.0f;

Shiny::Matrix4x4 Shiny::ThirdPersonCamera::GetViewMatrix(const Float3& focusPosition) const
{
    float sinTheta = std::sinf(theta_);
    float cosTheta = std::cosf(theta_);
    float sinPhi = std::sinf(phi_);
    float cosPhi = std::cosf(phi_);
    
    float x = sinTheta * cosPhi;
    float y = sinTheta * sinPhi;
    float z = cosTheta;
    
    Float3 axis = Cross(Float3(0, 1, 0), Float3(x, y, z));
    float angle = acos(Dot(Float3(0, 1, 0), Float3(x, y, z)));
    return MakeTranslationMatrix(focusPosition - Float3(20*x, 20*y,20*z)) * MakeRotationMatrix(axis, angle);
}

void Shiny::ThirdPersonCamera::AddForce(float horizontal, float vertical)
{
    phi_ += DegreesToRadians(horizontal);
    theta_ += DegreesToRadians(vertical);
}
