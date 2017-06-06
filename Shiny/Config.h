#pragma once
#include <string>
#include <map>
#include <vector>
namespace Shiny
{
class Config
{
public:
    Config(const std::string& fileName);
private:
    std::vector<char> data_;
};
}