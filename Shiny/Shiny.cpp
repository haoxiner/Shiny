#include "Display.h"
#include "PerformanceTimer.h"
#include "RenderingSystem.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Game.h"
#include "ResourceManager.h"
#include "MathUtil.h"
#include "FreeImage.h"
#include <fstream>
#include <iostream>

int TestDisplay()
{
    constexpr int xResolution = 800;
    constexpr int yResolution = 600;
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
        game.Update(deltaTime);
        game.Render();
        display.Update();
        if (input->Quit()) {
            display.Shutdown();
        }
    }
    return 0;
}

struct InputBlock
{
    Shiny::Float4 inputArg0;
};
void GPUwait()
{
    GLsync syncObject = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    GLenum ret = glClientWaitSync(syncObject, GL_SYNC_FLUSH_COMMANDS_BIT, 1000 * 1000 * 10000);
    if (/*ret == GL_WAIT_FAILED || */ret == GL_TIMEOUT_EXPIRED)
        std::cerr << ("glClientWaitSync failed./n") << std::endl;
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glDeleteSync(syncObject);
}
int TestComputShader()
{
    constexpr int xResolution = 800;
    constexpr int yResolution = 600;
    Shiny::Display display;
    if (!display.Startup(xResolution, yResolution, false)) {
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
    GLuint samplerID_;
    glCreateSamplers(1, &samplerID_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //glSamplerParameteri(samplerID_, GL_TEXTURE_MAX_LOD, 3);
    glBindSampler(0, samplerID_);

    w = 1024;
    h = 512;
    auto texSize = sizeof(float) * 3 * w * h;
    auto bitmap = FreeImage_AllocateT(FIT_RGBF, w, h);
    auto pixels = FreeImage_GetBits(bitmap);

    GLuint outputTextureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &outputTextureID);
    glTextureStorage2D(outputTextureID, 1, GL_RGBA32F, w, h);
    glBindImageTexture(0, outputTextureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    GLuint inputBuffer;
    glCreateBuffers(1, &inputBuffer);
    InputBlock inputBlock = { Shiny::Float4(w, h, 0.0, 0.0) };
    glNamedBufferStorage(inputBuffer, sizeof(InputBlock), &inputBlock, GL_MAP_WRITE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, inputBuffer);

    Shiny::ShaderProgram computeShaderProgram;
    computeShaderProgram.Startup(Shiny::ResourceManager::ReadFileToString("./Shaders/PreComputation.comp.glsl"));
    computeShaderProgram.Use();
    //GLuint buffer;
    //glCreateBuffers(1, &buffer);
    //int initData[2] = { 0,0 };
    //glNamedBufferStorage(buffer, sizeof(int) * 2, &initData, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    std::cerr << "before " << glGetError() << std::endl;
    glDispatchCompute(w/8, h/8, 1);
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    //GPUwait();
    //std::cerr << "afterB " << glGetError() << std::endl;

    //std::cerr << "output" << std::endl;
    
    glGetTextureImage(outputTextureID, 0, GL_RGB, GL_FLOAT, texSize, pixels);

    std::cerr << "afterC " << glGetError() << std::endl;
    auto result = FreeImage_Save(FIF_EXR, bitmap, "../../output.exr", EXR_DEFAULT);
    std::cerr << result << std::endl;
    display.Shutdown();
    return 0;
}
int main()
{
    TestDisplay();
    return 0;
}