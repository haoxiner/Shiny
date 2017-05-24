#pragma once
#include "MathUtil.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <utility>
namespace Shiny
{
class Entity
{
public:
    Float3 position_;
    Float3 rotation_;
    Float3 scale_;
    std::vector<std::pair<Mesh*, Material*>> models_;
};
}