#pragma once
#include "Platform.h"
#include <string>
#include <vector>
namespace Shiny
{
class Material
{
public:
    enum Type
    {
        CONDUCTOR, DIELECTRIC, CONDUCTOR_AND_DIELECTRIC
    };
    Material(const std::string& name);
    ~Material();
    void Bind();
private:
    std::vector<GLuint> textureIDList_;
};
}