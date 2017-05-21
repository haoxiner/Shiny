#include "Display.h"
#include "PerformanceTimer.h"
#include "RenderingSystem.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include <fstream>
#include <iostream>

short MapToShort(float value)
{
    constexpr float mapToPositive = 32767.0f;
    constexpr float mapToNegative = 32768.0f;
    return static_cast<short>(value >= 0 ? mapToPositive * value : mapToNegative * value);
}
int TestDisplay()
{
    constexpr int xResolution = 800;
    constexpr int yResolution = 600;
    Shiny::Display display;
    if (!display.Startup(xResolution, yResolution)) {
        return 1;
    }
    Shiny::RenderingSystem renderingSystem;
    renderingSystem.EnableDepthTest();
    renderingSystem.EnableCullFace();
    renderingSystem.SetViewport(0, 0, xResolution, yResolution);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    
    std::vector<short> positions = {
        MapToShort(0.5f), MapToShort(0.5f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(-0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(0.0f)
    };
    std::vector<short> normals = {
        MapToShort(0.5f), MapToShort(0.5f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(-0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(0.0f),
        MapToShort(0.5f), MapToShort(-0.5f), MapToShort(0.0f), MapToShort(0.0f)
    };
    //std::vector<float> positions = {
    //    (0.5f), (0.5f), (0.0f), (0.0f),
    //    (-0.5f), (-0.5f), (0.0f), (0.0f),
    //    (0.5f), (-0.5f), (0.0f), (0.0f)
    //};
    //std::vector<float> normals = {
    //    (0.5f), (0.5f), (0.0f), (0.0f),
    //    (-0.5f), (-0.5f), (0.0f), (0.0f),
    //    (0.5f), (-0.5f), (0.0f), (0.0f)
    //};
    std::vector<unsigned int> indices = { 0,1,2 };

    Shiny::Mesh mesh(2);
    mesh.LoadVertexAttribute(0, 4, true, positions);
    mesh.LoadVertexAttribute(1, 4, true, normals);
    mesh.LoadIndices(indices);
    

    std::ifstream vsFile("./Shaders/pbr.vert.glsl");
    std::istreambuf_iterator<char> vsBegin(vsFile), vsEnd;
    std::string vertexShaderSource(vsBegin, vsEnd);
    vsFile.close();
    vsFile.clear();
    std::ifstream fsFile("./Shaders/pbr.frag.glsl");
    std::istreambuf_iterator<char> fsBegin(fsFile), fsEnd;
    std::string fragmentShaderSource(fsBegin, fsEnd);
    fsFile.close();
    fsFile.clear();
    Shiny::ShaderProgram shaderProgram;
    shaderProgram.Startup(vertexShaderSource, fragmentShaderSource);
    
    // input device
    auto input = display.GetInput();
    // tick
    float deltaTime = 0.0f;
    Shiny::PerformanceTimer performanceTimer;
    performanceTimer.Reset();
    // game loop
    while (display.Running()) {
        performanceTimer.Tick();
        deltaTime = performanceTimer.GetDeltaTime();
        shaderProgram.Use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh.Render();
        display.Update();
        if (input->Quit()) {
            display.Shutdown();
        }
    }
    return 0;
}
int main()
{
    TestDisplay();
    return 0;
}