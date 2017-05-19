#pragma once
namespace Shiny
{
class VertexElementElementLayout
{
public:
    enum Format
    {
        FLOAT, SHORT, UNSIGNED_SHORT
    };
private:
    int numOfChannel_;
    Format format_;
    int startLocation_;
    int stride_;
};
class VertexBufferLayout
{
public:

private:
};
}