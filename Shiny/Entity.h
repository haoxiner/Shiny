#pragma once
#include "MathUtil.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <memory>
#include <map>
namespace Shiny
{
class Entity
{
public:
    Float3 position_;
    Quaternion rotation_;
    Float3 scale_;
    std::map<std::shared_ptr<Material>, std::vector<std::shared_ptr<Mesh>>> models_;
};
}