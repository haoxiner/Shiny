#pragma once
#include "MathUtil.h"
#include <memory>
namespace Shiny
{
struct Vertex
{
    Float3 p;
    Int_2_10_10_10 n;
    unsigned short tx;
    unsigned short ty;
    bool operator<(const Vertex& rhs) const
    {
        return std::memcmp((void*)this, (void*)&rhs, sizeof(Vertex)) < 0;
    };
};
}