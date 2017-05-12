#include "RenderingSystem.h"

void Shiny::RenderingSystem::EnableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void Shiny::RenderingSystem::DisableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
}

void Shiny::RenderingSystem::EnableCullFace()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Shiny::RenderingSystem::DisableCullFace()
{
    glDisable(GL_CULL_FACE);
}

void Shiny::RenderingSystem::SetViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void Shiny::RenderingSystem::SetMaterial(Material* material)
{
}

void Shiny::RenderingSystem::Render(Model* model)
{
    model->Render();
}
