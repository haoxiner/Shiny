#include "ThirdPersonCamera.h"
#include <iostream>
const float Shiny::ThirdPersonCamera::DEFAULT_THETA = Shiny::DegreesToRadians(90.0f);
const float Shiny::ThirdPersonCamera::DEFAULT_PHI = Shiny::DegreesToRadians(-90.0f);

void Shiny::ThirdPersonCamera::Update(float deltaTime, const Input& input, float forwardRotation)
{
    float pitchChange = input.GetRightVerticalAxis() * 0.5f * PI * deltaTime;
    //theta_ += pitchChange;
    theta_ = Clamp(theta_ + pitchChange, 1e-3f, 3.14f);
    //float hDis = distanceFromPlayer_ * std::cosf(pitch_);
    //float vDis = -distanceFromPlayer_ * std::sinf(pitch_);

    //    if (pitch_ > 0) {
    //        constexpr float invHalfPi = 1.0f / (PI * 0.5);
    //        distanceFromPlayer_ = (1.0f - pitch_ * invHalfPi) * MAX_DISTANCE_FROM_PLAYER;
    //        hDis = distanceFromPlayer_ * std::cosf(pitch_);
    //        vDis = -distanceFromPlayer_ * std::sinf(pitch_);
    //    } else {
    //        distanceFromPlayer_ = MAX_DISTANCE_FROM_PLAYER;
    //    }
    //float theta = glm::radians(-90.0f) - player->GetRotationToForword();
    //float offsetX = hDis * std::cosf(theta);
    //float offsetZ = hDis * std::sinf(theta);
    //position_.x = player->position_.x - offsetX;
    //position_.z = player->position_.z - offsetZ;
    //position_.y = player->position_.y + vDis + 10;
    //    yaw_ = glm::pi<float>() - (theta);
    phi_ = forwardRotation + DEFAULT_PHI;
}

void Shiny::ThirdPersonCamera::GetPose(Matrix4x4& viewMatrix, Float3& cameraPosition, const Float3& focusPosition)
{
    float sinTheta = std::sinf(theta_);
    float cosTheta = std::cosf(theta_);
    float sinPhi = std::sinf(phi_);
    float cosPhi = std::cosf(phi_);
    
    float x = sinTheta * cosPhi;
    float y = sinTheta * sinPhi;
    float z = cosTheta;

    auto Rotate90degreeAboutXAxis = Matrix4x4(
        1, 0, 0, 0,
        0, 0, -1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1
    );

    cameraPosition = focusPosition + Float3(x, y, z)*4.f;
    const auto f(Normalize(focusPosition - cameraPosition));
    const auto s(Normalize(Cross(f, Float3(0, 0, 1))));
    const auto u(cross(s, f));

    viewMatrix[0][0] = s.x;
    viewMatrix[1][0] = s.y;
    viewMatrix[2][0] = s.z;
    viewMatrix[0][1] = u.x;
    viewMatrix[1][1] = u.y;
    viewMatrix[2][1] = u.z;
    viewMatrix[0][2] = -f.x;
    viewMatrix[1][2] = -f.y;
    viewMatrix[2][2] = -f.z;
    viewMatrix[3][0] = -Dot(s, cameraPosition);
    viewMatrix[3][1] = -Dot(u, cameraPosition);
    viewMatrix[3][2] = Dot(f, cameraPosition);
}

void Shiny::ThirdPersonCamera::AddForce(float horizontal, float vertical)
{
    phi_ += DegreesToRadians(horizontal);
    theta_ = Clamp(theta_ + DegreesToRadians(vertical), 1e-3f, 3.14f);
}