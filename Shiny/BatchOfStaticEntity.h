#pragma once
#include "Entity.h"
#include "Material.h"
#include "MathUtil.h"
#include <vector>
namespace Shiny
{
class BatchOfStaticEntity
{
public:
private:
    std::vector<Entity> entityList_;
    Matrix4x4 modelToWorld_;
};
}