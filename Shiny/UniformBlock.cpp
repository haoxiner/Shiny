#include "UniformBlock.h"
#include <memory>

bool Shiny::UniformBlock::Startup()
{
    if (id_ != 0) {
        return false;
    }
    glGenBuffers(1, &id_);
    return true;
}

void Shiny::UniformBlock::Shutdown()
{
    if (id_ != 0) {
        glDeleteBuffers(1, &id_);
        id_ = 0;
    }
}

GLuint Shiny::UniformBlock::GetID() const
{
    return id_;
}

GLvoid* Shiny::DynamicUniformBlock::MapBuffer()
{
    return glMapNamedBuffer(id_, GL_WRITE_ONLY);
}

void Shiny::DynamicUniformBlock::UnMapBuffer()
{
    glUnmapNamedBuffer(id_);
}

bool Shiny::DynamicUniformBlock::Startup(const void* initializationData, int size)
{
    if (!UniformBlock::Startup()) {
        return false;
    }
    glBufferData(GL_UNIFORM_BUFFER, size, initializationData, GL_DYNAMIC_DRAW);
    return true;
}

void Shiny::DynamicUniformBlock::Update(const void* data, int size)
{
    void* mappedBufferLocation = MapBuffer();
    memcpy(mappedBufferLocation, data, size);
    UnMapBuffer();
}

bool Shiny::StaticUniformBlock::Startup(const void* initializationData, int size)
{
    if (!UniformBlock::Startup()) {
        return false;
    }
    glBufferData(GL_UNIFORM_BUFFER, size, initializationData, GL_STATIC_DRAW);
    return true;
}

void Shiny::StaticUniformBlock::Update(const void* data, int size)
{
    glNamedBufferSubData(id_, 0, size, data);
}
