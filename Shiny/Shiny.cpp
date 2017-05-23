#include "Display.h"
#include "PerformanceTimer.h"
#include "RenderingSystem.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Game.h"
#include "ResourceManager.h"
#include <fstream>
#include <iostream>

int TestDisplay()
{
    constexpr int xResolution = 800;
    constexpr int yResolution = 600;
    Shiny::Display display;
    if (!display.Startup(xResolution, yResolution)) {
        return 1;
    }
    // input device
    auto input = display.GetInput();
    // Game
    Shiny::Game game;
    game.Startup(xResolution, yResolution, input);
    // tick
    float deltaTime = 0.0f;
    Shiny::PerformanceTimer performanceTimer;
    performanceTimer.Reset();
    // game loop
    while (display.Running()) {
        performanceTimer.Tick();
        deltaTime = performanceTimer.GetDeltaTime();
        game.Update(deltaTime);
        game.Render();
        display.Update();
        if (input->Quit()) {
            display.Shutdown();
        }
    }
    return 0;
}
int TestComputShader()
{
    constexpr int xResolution = 800;
    constexpr int yResolution = 600;
    Shiny::Display display;
    if (!display.Startup(xResolution, yResolution)) {
        return 1;
    }

    Shiny::ShaderProgram computeShaderProgram;
    computeShaderProgram.Startup(Shiny::ResourceManager::ReadFileToString("./Shaders/cullLight.comp.glsl"));
    computeShaderProgram.Use();
    GLuint buffer;
    glCreateBuffers(1, &buffer);
    int initData = 0;
    glNamedBufferStorage(buffer, sizeof(int), &initData, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    auto pBuffer = glMapNamedBuffer(buffer, GL_WRITE_ONLY);
    *(int*)pBuffer = 1024;
    glUnmapNamedBuffer(buffer);

    glDispatchCompute(1, 1, 1);
    pBuffer = glMapNamedBuffer(buffer, GL_READ_ONLY);
    std::cerr << *(int*)pBuffer << std::endl;
    glUnmapNamedBuffer(buffer);
    display.Shutdown();
    return 0;
}
int main()
{
    TestComputShader();
    return 0;
}