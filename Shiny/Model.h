#pragma once
#include "Platform.h"
#include <vector>
#include <cstdint>
namespace Shiny
{
class Model
{
public:
    bool Startup(std::vector<float>& positions,
                 std::vector<float>& normals,
                 std::vector<float>& texCoords,
                 std::vector<unsigned short>& indices);
    bool Startup(std::vector<short>& vertexBuffer,
                 std::vector<unsigned short>& indices);
    void Shutdown();
    void Render();
protected:
    GLuint CreateVAO();
    void UnbindVAO();
    void StoreDataInAttributeList(int attributeLocation, int elementCountPerVertex, std::vector<float>& data);
    void BindIndicesBuffer(std::vector<unsigned short>& indices);
private:
    GLuint vao_;
    std::vector<GLuint> vboList_;
    GLsizei indexCount_;
};
}