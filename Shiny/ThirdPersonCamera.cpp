#include "ThirdPersonCamera.h"
#include <iostream>
const float Shiny::ThirdPersonCamera::DEFAULT_THETA = Shiny::DegreesToRadians(90.0f);
const float Shiny::ThirdPersonCamera::DEFAULT_PHI = Shiny::DegreesToRadians(-90.0f);

void Shiny::ThirdPersonCamera::GetPose(Matrix4x4& viewMatrix, Float3& cameraPosition, const Float3& focusPosition)
{
    float sinTheta = std::sinf(theta_);
    float cosTheta = std::cosf(theta_);
    float sinPhi = std::sinf(phi_);
    float cosPhi = std::cosf(phi_);
    
    float x = sinTheta * cosPhi;
    float y = sinTheta * sinPhi;
    float z = cosTheta;
    
    //Float3 axis = Cross(Float3(0, 1, 0), Float3(x, y, z));
    //float angle = acos(Dot(Float3(0, 1, 0), Float3(x, y, z)));
    //return MakeTranslationMatrix(focusPosition - Float3(20*x, 20*y,20*z)) * MakeRotationMatrix(axis, angle);
    auto eye = focusPosition + Float3(x, y, z) * 40.0f;
    //std::cerr << eye.x << "," << eye.y << "," << eye.z << std::endl;
    //std::cerr << x << "," << y << "," << z << std::endl;
    
    cameraPosition = Float3(0, -400, 100);
    viewMatrix = MakeTranslationMatrix(-cameraPosition);
}

void Shiny::ThirdPersonCamera::AddForce(float horizontal, float vertical)
{
    phi_ += DegreesToRadians(horizontal);
    theta_ += DegreesToRadians(vertical);
}
