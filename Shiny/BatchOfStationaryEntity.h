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
    std::vector<Entity> entityList_;
};
}