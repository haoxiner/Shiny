#pragma once
#include "Platform.h"
#include <vector>
namespace Shiny
{
class Mesh
{
public:
    Mesh(int numOfAttibutePerVertex);
    ~Mesh();
    void LoadVertexAttribute(int index, int numOfChannel, bool normalized, const std::vector<short>& data);
    void LoadVertexAttribute(int index, int numOfChannel, const std::vector<float>& data);
    void LoadIndices(const std::vector<unsigned short>& indices);
    void LoadIndices(const std::vector<unsigned int>& indices);
    void LoadStandardPackage(const std::string& name);
    void Render();
private:
    void LoadIndices(const void* data, int size);
    GLuint vao_;
    std::vector<GLuint> vboList_;
    int numOfIndex_;
    bool indexIsUnsignedShort;
};
}