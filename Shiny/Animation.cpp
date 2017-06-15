#include "Animation.h"
#include "MathUtil.h"
#include <fstream>
#include <iostream>
#include <vector>

Shiny::Animation::Animation(const std::string& name)
{
    glCreateBuffers(1, &animationFrameBufferID_);
    std::ifstream inputFileStream("../../Resources/Model/run.anim", std::ios::binary);

    std::streampos fsize = 0;
    auto fstart = inputFileStream.tellg();
    inputFileStream.seekg(0, std::ios::end);
    fsize = inputFileStream.tellg() - fstart;

    inputFileStream.seekg(0, std::ios::beg);

    std::vector<Float4> data(fsize);
    std::cerr << "ANIM BEGIN" << std::endl;
    std::cerr << (data.size() * sizeof(Float4)) << ", " << fsize << std::endl;
    inputFileStream.read((char*)data.data(), fsize);
    inputFileStream.close();

    std::cerr << "" << fsize << std::endl;
    glNamedBufferStorage(animationFrameBufferID_, fsize, data.data(), 0);
}

void Shiny::Animation::Use()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, animationFrameBufferID_);
}
