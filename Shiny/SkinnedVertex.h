#pragma once
#include "MathUtil.h"
#include <memory>

namespace Shiny
{
struct SkinnedVertex
{
    Float3 p;
    Int_2_10_10_10 n;
    unsigned short tx;
    unsigned short ty;
    unsigned short bone[4];
    unsigned short weight[4];
    bool operator<(const SkinnedVertex& rhs) const
    {
        return std::memcmp((void*)this, (void*)&rhs, sizeof(SkinnedVertex)) < 0;
    };
};
}