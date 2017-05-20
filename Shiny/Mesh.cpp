#include "Mesh.h"

Shiny::Mesh::Mesh(int numOfAttibutePerVertex)
{
    glGenVertexArrays(1, &vao_);
    vboList_.resize(numOfAttibutePerVertex + 1);
    glGenBuffers(vboList_.size(), vboList_.data());
}

Shiny::Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(vboList_.size(), vboList_.data());
}

void Shiny::Mesh::LoadVertexAttribute(int index, int numOfChannel, bool normalized, const std::vector<short>& data)
{
    glNamedBufferData(vboList_[index], data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    glVertexArrayAttribFormat(vao_, index, 4, numOfChannel, normalized ? GL_TRUE : GL_FALSE, 0);
}

void Shiny::Mesh::LoadIndices(const std::vector<unsigned short>& indices)
{
    indexIsUnsignedShort = true;
    LoadIndices(indices.data(), indices.size() * sizeof(indices[0]));
}

void Shiny::Mesh::LoadIndices(const std::vector<unsigned int>& indices)
{
    indexIsUnsignedShort = false;
    LoadIndices(indices.data(), indices.size() * sizeof(indices[0]));
}

void Shiny::Mesh::Render()
{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, numOfIndex_, indexIsUnsignedShort ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Shiny::Mesh::LoadIndices(const void* data, int size)
{
    glNamedBufferData(vboList_.back(), size, data, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao_, vboList_.back());
}
