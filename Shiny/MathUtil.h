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

/*
* map float of [-1.0, 1.0] to short
*/
inline short MapToShort(float value)
{
    constexpr float mapToPositive = 32767.0f;
    constexpr float mapToNegative = 32768.0f;
    return static_cast<short>((value >= 0 ? mapToPositive * value : mapToNegative * value) + 0.5f);
}

using Float2 = glm::vec2;
using Float3 = glm::vec3;
using Float4 = glm::vec4;
using Quaternion = Float4;
using Matrix4x4 = glm::mat4;

/*
* Make ViewToProjection Transform
*/
inline Matrix4x4 MakePerspectiveProjectionMatrix(float verticalFov, float aspect, float zNear, float zFar)
{
    return glm::perspective(verticalFov, aspect, zNear, zFar);
}

/*
* Quaternion To Matrix4x4
*/
inline Matrix4x4 QuaternionToMatrix(const Quaternion& quaternion)
{
    float xy = quaternion.x * quaternion.y;
    float yz = quaternion.y * quaternion.z;
    float zx = quaternion.z * quaternion.x;
    float x2 = quaternion.x * quaternion.x;
    float y2 = quaternion.y * quaternion.y;
    float z2 = quaternion.z * quaternion.z;
    float xw = quaternion.x * quaternion.w;
    float yw = quaternion.y * quaternion.w;
    float zw = quaternion.z * quaternion.w;
    return Matrix4x4(1.0 - 2.0*(y2 + z2), 2.0*(xy - zw), 2.0*(zx + yw), 0.0,
                     2.0*(xy + zw), 1.0 - 2.0*(x2 + z2), 2.0*(yz - xw), 0.0,
                     2.0*(zx - yw), 2.0*(yz + xw), 1.0 - 2.0*(x2 + y2), 0.0,
                     0.0, 0.0, 0.0, 1.0);
}

/*
* Translate By XYZ
*/
inline Matrix4x4 MakeTranslationMatrix(float x, float y, float z)
{
    return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     x, y, z, 1.0f);
}
/*
* Translate By Vector3f
*/
inline Matrix4x4 MakeTranslationMatrix(const Float3& vector3f)
{
    return MakeTranslationMatrix(vector3f.x, vector3f.y, vector3f.z);
}
}