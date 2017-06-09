#pragma once
#include "Entity.h"
#include "Material.h"
#include <vector>
#include <map>
#include <memory>
namespace Shiny
{
class BatchOfStationaryEntity
{
public:
    
private:
    std::map<std::shared_ptr<Material>, std::vector<Entity>> entityMap_;
};
}