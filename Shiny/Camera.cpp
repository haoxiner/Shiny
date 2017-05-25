#include "Camera.h"

const Shiny::Float3& Shiny::Camera::GetPosition() const
{
    return position_;
}

const Shiny::Matrix4x4& Shiny::Camera::GetViewMatrix() const
{
    Quaternion cameraRotation(0.0f, 0.0f, 0.0f, 0.0f);
    
    return Matrix4x4();
}
