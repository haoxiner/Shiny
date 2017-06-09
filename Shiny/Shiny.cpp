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
    constexpr int xResolution = 1440;
    constexpr int yResolution = 900;
    Shiny::Display display;
    if (!display.Startup(xResolution, yResolution, true)) {
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
        game.Update(deltaTime, input);
        game.Render();
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
//void TestPreComputation()
//{
//    constexpr int xResolution = 800;
//    constexpr int yResolution = 600;
//    Shiny::Display display;
//    if (!display.Startup(xResolution, yResolution, false)) {
//        return;
//    }
//    Shiny::PreComputation preComputation;
//    //preComputation.LoadSourceFile("../../uffizi.exr");
//    preComputation.ComputeIBLDFG("../../output/dfg.exr");
//    preComputation.LoadSourceFile("pisa");
//    preComputation.ComputeIBLDiffuse("../../output/pisa_diffuse");
//    preComputation.ComputeIBLSpecular("../../output/pisa_specular");
//}

void TestPreIntegrator()
{
    constexpr int xResolution = 800;
    constexpr int yResolution = 600;
    Shiny::Display display;
    if (!display.Startup(xResolution, yResolution, false)) {
        return;
    }
    
    Shiny::PreIntegrator preIntegrator;
    //preIntegrator.IntegrateIBLDFG("dfg", "../../Resources/Environment");
    preIntegrator.IntegrateIBLDiffuseAndSpecular("../../Resources/Environment/uffizi/src", "uffizi", "../../Resources/Environment/uffizi", "uffizi");
}
int main()
{
    //TestPreIntegrator();
    using namespace Shiny;
    TestDisplay();
    return 0;
}