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
    computeShaderProgram.Startup(Shiny::ResourceManager::ReadFileToString("./Shaders/cullLight.compute.glsl"));
    computeShaderProgram.Use();
    //GLuint buffer;
    //glCreateBuffers(1, &buffer);
    //glNamedBufferStorage(buffer, sizeof(float), nullptr, GL_MAP_WRITE_BIT);
    //glDispatchCompute(1, 1, 1);
    display.Shutdown();
    return 0;
}
int main()
{
    TestComputShader();
    return 0;
}