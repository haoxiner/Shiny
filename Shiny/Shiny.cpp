#include "Display.h"
#include "PerformanceTimer.h"
#include "RenderingSystem.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Game.h"
#include "ResourceManager.h"
#include "FreeImage.h"
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


    GLuint textureID_;
    FIBITMAP* dib = FreeImage_Load(FIF_EXR, "../../uffizi.exr");
    auto colorType = FreeImage_GetColorType(dib);
    auto bpp = FreeImage_GetBPP(dib);
    auto w = FreeImage_GetWidth(dib);
    auto h = FreeImage_GetHeight(dib);
    std::cerr << w << "," << h << std::endl;
    auto bits = FreeImage_GetBits(dib);
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID_);
    glTextureStorage2D(textureID_, 1, GL_RGB32F, w, h);
    glTextureSubImage2D(textureID_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
    FreeImage_Unload(dib);
    glBindTextureUnit(0, textureID_);

    //auto w = 512, h = 512;
    GLuint outputTextureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &outputTextureID);
    glTextureStorage2D(outputTextureID, 1, GL_RGBA32F, w, h);
    glBindImageTexture(0, outputTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


    Shiny::ShaderProgram computeShaderProgram;
    computeShaderProgram.Startup(Shiny::ResourceManager::ReadFileToString("./Shaders/PreComputation.comp.glsl"));
    computeShaderProgram.Use();
    //GLuint buffer;
    //glCreateBuffers(1, &buffer);
    //int initData[2] = { 0,0 };
    //glNamedBufferStorage(buffer, sizeof(int) * 2, &initData, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);

    glDispatchCompute(w, h, 1);
    std::cerr << "output" << std::endl;
    auto texSize = sizeof(float) * 3 * w * h;
    auto bitmap = FreeImage_AllocateT(FIT_RGBF,w, h);
    auto pixels = FreeImage_GetBits(bitmap);
    //new BYTE[bufSize];
    std::cerr << (pixels ? 1 : 0) << std::endl;

    glGetTextureImage(outputTextureID, 0, GL_RGB, GL_FLOAT, texSize, pixels);
    
    auto result = FreeImage_Save(FIF_EXR, bitmap, "../../output.exr", EXR_DEFAULT);
    std::cerr << result << std::endl;
    display.Shutdown();
    return 0;
}
int main()
{
    TestComputShader();
    return 0;
}