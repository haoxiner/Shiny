#pragma once
#include "Cubemap.h"
#include "MathUtil.h"
#include "ShaderProgram.h"
#include "Platform.h"
#include <string>
namespace Shiny
{
class PreIntegrator
{
public:
    void IntegrateIBLDFG(const std::string& outputID, const std::string& outputPath);
    void IntegrateIBLDiffuseAndSpecular(const std::string& inputID, const std::string& inputDirectory, const std::string& outputID, const std::string& outputDirectory);
private:
    void Save(GLuint textureID, const std::string& fileName, int width, int height, bool invertHorizontal, bool invertVertical);
    struct ArgumentsBlock
    {
        Shiny::Float4 inputArg0;
        Shiny::Float4 inputArg1;
    };
};
}