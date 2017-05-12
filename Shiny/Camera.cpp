#include "Camera.h"

const Shiny::Vector3f& Shiny::Camera::GetPosition() const
{
    return position_;
}

const Shiny::Matrix4x4f& Shiny::Camera::GetViewMatrix() const
{
    Quaternion cameraRotation(0.0f, 0.0f, 0.0f, 0.0f);
    
    return Matrix4x4f();
}
