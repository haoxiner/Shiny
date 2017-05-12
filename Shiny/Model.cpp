#include "Model.h"

bool Shiny::Model::Startup(std::vector<float>& positions, std::vector<float>& normals, std::vector<float>& texCoords, std::vector<unsigned short>& indices)
{
    vao_ = CreateVAO();
    indexCount_ = static_cast<GLsizei>(indices.size());
    StoreDataInAttributeList(0, 3, positions);
    StoreDataInAttributeList(1, 3, normals);
    StoreDataInAttributeList(2, 2, texCoords);
    BindIndicesBuffer(indices);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    UnbindVAO();
    return true;
}

bool Shiny::Model::Startup(std::vector<short>& vertexBuffer, std::vector<unsigned short>& indices)
{
    vao_ = CreateVAO();
    indexCount_ = static_cast<GLsizei>(indices.size());
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(vertexBuffer[0]), static_cast<void*>(vertexBuffer.data()), GL_STATIC_DRAW);

    int stride = 16 * sizeof(short);
    glVertexAttribPointer(0, 4, GL_SHORT, GL_FALSE, stride, 0);
    glVertexAttribPointer(1, 4, GL_SHORT, GL_TRUE, stride, (void*)(4 * sizeof(short)));
    glVertexAttribPointer(2, 4, GL_UNSIGNED_SHORT, GL_FALSE, stride, (void*)(8 * sizeof(short)));
    glVertexAttribPointer(3, 4, GL_UNSIGNED_SHORT, GL_TRUE, stride, (void*)(12 * sizeof(short)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vboList_.push_back(vbo);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
    vboList_.push_back(vbo);
    glEnableVertexAttribArray(0);
    UnbindVAO();
    return true;
}

void Shiny::Model::Shutdown()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(static_cast<GLsizei>(vboList_.size()), static_cast<GLuint*>(vboList_.data()));
}

void Shiny::Model::Render()
{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_SHORT, (void*)0);
    glBindVertexArray(0);
}

GLuint Shiny::Model::CreateVAO()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    return vao;
}

void Shiny::Model::UnbindVAO()
{
    glBindVertexArray(0);
}

void Shiny::Model::StoreDataInAttributeList(int attributeLocation, int elementCountPerVertex, std::vector<float>& data)
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), static_cast<void*>(data.data()), GL_STATIC_DRAW);
    glVertexAttribPointer(attributeLocation, elementCountPerVertex, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vboList_.push_back(vbo);
}

void Shiny::Model::BindIndicesBuffer(std::vector<unsigned short>& indices)
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
    vboList_.push_back(vbo);
}
