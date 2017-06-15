#include "Display.h"
#include "PerformanceTimer.h"
#include "RenderingSystem.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Game.h"
#include "ResourceManager.h"
#include "MathUtil.h"
#include "PreIntegrator.h"
#include "Config.h"
#include <FreeImage.h>
#include <fstream>
#include <iostream>

int TestDisplay()
{
    constexpr int xResolution = 1024;
    constexpr int yResolution = 768;
    Shiny::Display display;
    if (!display.Startup(xResolution, yResolution, true)) {
        return 1;
    }
    // input device
    auto input = display.GetInput();
    // Game
    Shiny::Game game;
    if (!game.Startup(xResolution, yResolution, input)) {
        return 1;
    }
    // tick
    float deltaTime = 0.0f;
    Shiny::PerformanceTimer performanceTimer;
    performanceTimer.Reset();
    // game loop
    while (display.Running()) {
        performanceTimer.Tick();
        deltaTime = performanceTimer.GetDeltaTime();
        game.Update(deltaTime, input);
        display.Update();
        if (input->Quit()) {
            display.Shutdown();
        }
    }
    return 0;
}
void GPUwait()
{
    GLsync syncObject = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    GLenum ret = glClientWaitSync(syncObject, GL_SYNC_FLUSH_COMMANDS_BIT, 1000 * 1000 * 1000);
    if (/*ret == GL_WAIT_FAILED || */ret == GL_TIMEOUT_EXPIRED)
        std::cerr << ("glClientWaitSync failed./n") << std::endl;
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glDeleteSync(syncObject);
}

void TestPreIntegrator()
{
    constexpr int xResolution = 800;
    constexpr int yResolution = 600;
    Shiny::Display display;
    if (!display.Startup(xResolution, yResolution, false)) {
        return;
    }
    
    Shiny::PreIntegrator preIntegrator;
    preIntegrator.IntegrateIBLDFG("dfg", "../../Resources/Environment");
    //preIntegrator.IntegrateIBLDiffuseAndSpecular("../../Resources/Environment/uffizi/src", "uffizi", "../../Resources/Environment/uffizi", "uffizi");
}
int main()
{
    //TestPreIntegrator();
    using namespace Shiny;
    ResourceManager::WriteObjToSPK("../../Resources/Model/prototype.obj", "../../Resources/Model/prototype.bin");
    TestDisplay();
    return 0;
}