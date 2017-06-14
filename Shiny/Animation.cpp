#include "Animation.h"
#include "MathUtil.h"
#include <fstream>
#include <iostream>
#include <vector>

Shiny::Animation::Animation(const std::string& name)
{
    glCreateBuffers(1, &animationFrameBufferID_);
    std::ifstream inputFileStream("../../Resources/Model/prototype.anim", std::ios::binary);

    std::streampos fsize = 0;
    auto fstart = inputFileStream.tellg();
    inputFileStream.seekg(0, std::ios::end);
    fsize = inputFileStream.tellg() - fstart;

    inputFileStream.seekg(std::ios::beg);

    std::vector<Float4> data(3 * 60 * 21);
    std::cerr << "ANIM BEGIN" << std::endl;
    std::cerr << (data.size() * sizeof(Float4)) << ", " << fsize << std::endl;
    inputFileStream.read((char*)data.data(), fsize);
    inputFileStream.close();

    //for (int j = 0; j < 60; j++) {
    //    data[j * 3] = (Float4(1, 0, 0, 0));
    //    data[j * 3 + 1] = (Float4(0, 1, 0, 0));
    //    data[j * 3 + 2] = (Float4(0, 0, 1, 0));
    //}
    //inputFileStream.read((char*)data.data(), fsize);

    std::cerr << "" << fsize << std::endl;
    glNamedBufferStorage(animationFrameBufferID_, fsize, data.data(), 0);
}

void Shiny::Animation::Use()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, animationFrameBufferID_);
}
