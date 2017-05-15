#pragma once
#include "Platform.h"
#include "MathUtil.h"
#include <string>
namespace Shiny
{
class ShaderProgram
{
public:
    void Use();
    void Shutdown();
protected:
    bool Startup(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    bool Startup(const std::string& computeShaderSource);
    virtual void BindAttributes() {};
    virtual void GetAllUniformLocations() {};
    void BindAttributeLocation(GLuint attributeLocation, const std::string& name);
    GLint GetUniformLocation(const std::string& name);
    void LoadInteger(GLint location, int value);
    void LoadFloat(GLint location, float value);
    void LoadVector(GLint location, const Float3& value);
    void LoadMatrix(GLint location, const Matrix4x4& value);
private:
    GLuint LoadShader(const std::string& source, GLenum type);
    GLuint program_;

    // opengl 4.5
public:
};
}