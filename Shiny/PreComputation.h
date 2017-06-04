#pragma once
#include "MathUtil.h"
#include "ShaderProgram.h"
#include "Cubemap.h"
#include "Platform.h"
#include <string>
namespace Shiny
{
class PreComputation
{
public:
    PreComputation();
    ~PreComputation();
    void LoadSourceFile(const std::string& fileName);
    void ComputeIBLDFG(const std::string& destFileName);
    void ComputeIBLDiffuse(const std::string& destFileName);
    void ComputeIBLSpecular(const std::string& destFileName);
private:
    void Save(const std::string& fileName, int width, int height, bool invertHorizontal = false, bool invertVertical = false);
    //GLuint srcTextureID_;
    int srcMaxLevel_ = 0;
    GLuint destTextureID_;
    //GLuint linearSamplerID_;
    GLuint inputBufferID_;
    struct InputBlock
    {
        Shiny::Float4 inputArg0;
        Shiny::Float4 inputArg1;
    };
    std::string shaderSource_;

    Cubemap* cubemap_;
};
}