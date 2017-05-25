#include "Mesh.h"
#include <iostream>

Shiny::Mesh::Mesh(int numOfAttibutePerVertex)
{
    glCreateVertexArrays(1, &vao_);
    vboList_.resize(numOfAttibutePerVertex + 1);
    glCreateBuffers(vboList_.size(), vboList_.data());
    //glGenVertexArrays(1, &vao_);
    //glGenBuffers(vboList_.size(), vboList_.data());
}

Shiny::Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(vboList_.size(), vboList_.data());
}

void Shiny::Mesh::LoadVertexAttribute(int index, int numOfChannel, bool normalized, const std::vector<short>& data)
{
    //glBindVertexArray(vao_);
    //glEnableVertexAttribArray(index);
    //glBindBuffer(GL_ARRAY_BUFFER, vboList_[index]);
    //glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    //glVertexAttribPointer(index, numOfChannel, GL_SHORT, GL_TRUE, 0, nullptr);
    //glBindVertexArray(0);
    //return;
    if (index >= vboList_.size() - 1) {
        return;
    }
    glNamedBufferStorage(vboList_[index], sizeof(data[0]) * data.size(), data.data(), 0);
    //glNamedBufferData(vboList_[index], data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao_, index);
    glVertexArrayVertexBuffer(vao_, index, vboList_[index], 0, numOfChannel * sizeof(data[0]));
    if (normalized) {
        glVertexArrayAttribFormat(vao_, index, numOfChannel, GL_SHORT, GL_TRUE, 0);
    } else {
        glVertexArrayAttribIFormat(vao_, index, numOfChannel, GL_SHORT, 0);
    }
    glVertexArrayAttribBinding(vao_, index, index);

    //glBindVertexArray(vao_);
    //glEnableVertexAttribArray(index);
    //glBindBuffer(GL_ARRAY_BUFFER, vboList_[index]);
    //glVertexAttribPointer(index, numOfChannel, GL_SHORT, GL_TRUE, 0, nullptr);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
}

void Shiny::Mesh::LoadVertexAttribute(int index, int numOfChannel, const std::vector<float>& data)
{
    //glBindVertexArray(vao_);
    //glEnableVertexAttribArray(index);
    //glBindBuffer(GL_ARRAY_BUFFER, vboList_[index]);
    //glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    //glVertexAttribPointer(index, numOfChannel, GL_FLOAT, GL_FALSE, 0, nullptr);
    //glBindVertexArray(0);
    //return;
    if (index >= vboList_.size() - 1) {
        return;
    }
    glNamedBufferStorage(vboList_[index], sizeof(data[0]) * data.size(), data.data(), 0);
    //glNamedBufferData(vboList_[index], data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao_, index);
    glVertexArrayVertexBuffer(vao_, index, vboList_[index], 0, numOfChannel * sizeof(data[0]));
    glVertexArrayAttribFormat(vao_, index, numOfChannel, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao_, index, 0);
}

void Shiny::Mesh::LoadIndices(const std::vector<unsigned short>& indices)
{
    indexIsUnsignedShort = true;
    numOfIndex_ = indices.size();
    LoadIndices(indices.data(), indices.size() * sizeof(indices[0]));
}

void Shiny::Mesh::LoadIndices(const std::vector<unsigned int>& indices)
{
    indexIsUnsignedShort = false;
    numOfIndex_ = indices.size();
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
    //glBindVertexArray(vao_);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboList_.back());
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    //glBindVertexArray(0);
    //return;
    glNamedBufferStorage(vboList_.back(), size, data, 0);
    //glNamedBufferData(vboList_.back(), size, data, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao_, vboList_.back());
}
