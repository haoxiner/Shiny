#include "Display.h"
#include "PerformanceTimer.h"
#include "RenderingSystem.h"
#include "GLSLProgram.h"
#include "UniformBlockDescription.h"
#include "VertexAttributeDescription.h"

int TestGLSL()
{
    Shiny::GLSLProgram program;
    program.Startup(
    {
        { Shiny::VertexAttributeDescription::FLOAT4, "P" }
    },
    {
        {
            "uniforms",
            {
                { Shiny::UniformBlockElementDescription::FLOAT4, "color", 0 }
            }
        }
    }, "void VS() {}", "vec4 PS() { return color; }");
    //program.Startup(
    //{
    //    { Shiny::VertexAttributeDescription::FLOAT4, "PTexU" },
    //    { Shiny::VertexAttributeDescription::FLOAT4, "NTexV" }
    //},
    //{
    //    {
    //        "perFrameBlock",
    //        {
    //            { Shiny::UniformBlockElementDescription::MAT4, "View", 0 }
    //        }
    //    },
    //    {
    //        "perObjectBlock",
    //        {
    //            { Shiny::UniformBlockElementDescription::MAT4, "Model", 0 },
    //            { Shiny::UniformBlockElementDescription::FLOAT4, "InverseQuantization", 0 },
    //            { Shiny::UniformBlockElementDescription::MAT4, "Projection", 0 }
    //        }
    //    },
    //}, "void VS() {}", "vec4 PS() { return vec4(0.0); }");
    return 0;
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

    auto input = display.GetInput();

    float deltaTime = 0.0f;

    // Test GLSL
    Shiny::GLSLProgram program;
    program.Startup(
    {
        { Shiny::VertexAttributeDescription::FLOAT4, "P" }
    },
    {
        {
            "uniforms",
            {
                { Shiny::UniformBlockElementDescription::FLOAT4, "color", 0 }
            }
        }
    }, "void VS() {}", "vec4 PS() { return color; }");

    // tick
    Shiny::PerformanceTimer performanceTimer;
    performanceTimer.Reset();
    // game loop
    while (display.Running()) {
        performanceTimer.Tick();
        deltaTime = performanceTimer.GetDeltaTime();
        display.Update();
        if (input->Quit()) {
            display.Shutdown();
        }
    }
    return 0;
}
int main()
{
    TestGLSL();
    return 0;
}