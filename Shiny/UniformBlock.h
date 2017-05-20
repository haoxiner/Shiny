#pragma once
#include "Platform.h"
namespace Shiny
{
class UniformBlock
{
public:
    void Shutdown();
    virtual void Update(const void* data, int size) = 0;
    GLuint GetID() const;
protected:
    bool Startup();
    GLuint id_ = 0;
};
class DynamicUniformBlock : public UniformBlock
{
public:
    bool Startup(const void* initializationData, int size);
    void Update(const void* data, int size) override;
private:
    GLvoid* MapBuffer();
    void UnMapBuffer();
};
class StaticUniformBlock : public UniformBlock
{
public:
    bool Startup(const void* initializationData, int size);
    void Update(const void* data, int size) override;
};
}