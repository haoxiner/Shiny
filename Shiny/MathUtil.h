#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

namespace Shiny
{
/*
* PI
*/
constexpr float PI = glm::pi<float>();
constexpr float TWO_PI = 2.0f * PI;
constexpr float INV_PI = 1.0f / PI;
constexpr float INV_TWO_PI = 1.0f / TWO_PI;

/*
* Wrap an angle to [-pi, pi]
* Angle must be close to [-pi, pi], otherwise the performance will be low
*/
inline float WrapAngle(float angle)
{
    //    const float invTwoPi = 1.0f / twoPi;
    //    return angle - twoPi * std::floorf(angle * invTwoPi);
    while (angle > PI) {
        angle -= TWO_PI;
    }
    while (angle < -PI) {
        angle += TWO_PI;
    }
    return angle;
}

/*
* degree to radians
*/
inline float DegreesToRadians(float degrees)
{
    return glm::radians(degrees);
}

using Float2 = glm::vec2;
using Float3 = glm::vec3;
using Float4 = glm::vec4;
using Quaternion = glm::quat;
using Matrix4x4 = glm::mat4;

/*
* quaternion to matrix4x4
*/
inline Matrix4x4 QuaternionToMatrix(const Quaternion& quaternion)
{
    return glm::mat4_cast(quaternion);
}

inline Matrix4x4 MakeTranslationMatrix(const Float3& vec3f)
{
    Matrix4x4 identity(1.0f);
    return glm::translate(identity, vec3f);
}
}