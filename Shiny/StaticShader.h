#pragma once
#include "ShaderProgram.h"
#include "Camera.h"
namespace Shiny {
class StaticShader : public ShaderProgram {
public:
    bool Startup();
    virtual void BindAttributes();
    virtual void GetAllUniformLocations();
    void LoadModelMatrix(const Matrix4x4& value);
    //void LoadViewMatrix(const Camera& camera);
    void LoadProjectionMatrix(const Matrix4x4& value);
private:
    GLint modelMatrixLocation_;
    GLint viewMatrixLocation_;
    GLint projectionMatrixLocation_;
};
}