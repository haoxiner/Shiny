#pragma once
#include "Platform.h"
#include <vector>
namespace Shiny
{
struct VertexDescription
{
    int numOfChannel_;
    GLenum type_;
    bool normalized_;
    int size_;
    VertexDescription(int numOfChannel,
                      GLenum type,
                      bool normalized,
                      int size) :
        numOfChannel_(numOfChannel), type_(type), normalized_(normalized), size_(size)
    {
    }
};
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
    void LoadVertices(const void* data, int size, const std::vector<VertexDescription>& descList);
    void Render();
    int GetNumOfIndex() { return numOfIndex_; }
private:
    void LoadIndices(const void* data, int size);
    GLuint vao_;
    std::vector<GLuint> vboList_;
    int numOfIndex_;
    bool indexIsUnsignedShort = false;
};

}