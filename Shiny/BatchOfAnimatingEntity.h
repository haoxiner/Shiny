#pragma once
#include "Entity.h"
#include "Material.h"
#include <vector>
#include <map>
#include <memory>
#include "Animation.h"
namespace Shiny
{
class BatchOfAnimatingEntity
{
public:
    std::map<std::shared_ptr<Animation>, std::vector<std::shared_ptr<Entity>>> batch_;
};
}