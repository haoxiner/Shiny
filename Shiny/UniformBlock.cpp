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

void Shiny::UniformBlock::BindBuffer()
{
    glBindBuffer(GL_UNIFORM_BUFFER, id_);
}

void Shiny::UniformBlock::UnBindBuffer()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

GLvoid* Shiny::DynamicUniformBlock::MapBuffer()
{
    return glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
}

void Shiny::DynamicUniformBlock::UnMapBuffer()
{
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

bool Shiny::DynamicUniformBlock::Startup(const void* initializationData, int size)
{
    if (!UniformBlock::Startup()) {
        return false;
    }
    BindBuffer();
    glBufferData(GL_UNIFORM_BUFFER, size, initializationData, GL_DYNAMIC_DRAW);
    UnBindBuffer();
    return true;
}

void Shiny::DynamicUniformBlock::Update(const void* data, int size)
{
    BindBuffer();
    void* mappedBufferLocation = MapBuffer();
    memcpy(mappedBufferLocation, data, size);
    UnMapBuffer();
    UnBindBuffer();
}

bool Shiny::StaticUniformBlock::Startup(const void* initializationData, int size)
{
    if (!UniformBlock::Startup()) {
        return false;
    }
    BindBuffer();
    glBufferData(GL_UNIFORM_BUFFER, size, initializationData, GL_STATIC_DRAW);
    UnBindBuffer();
    return true;
}

void Shiny::StaticUniformBlock::Update(const void* data, int size)
{
    BindBuffer();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    //glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    UnBindBuffer();
}
