#include "Mesh.h"
#include "Json.h"
#include "ResourceManager.h"
#include <iostream>

Shiny::Mesh::Mesh(int numOfAttibutePerVertex)
{
    glCreateVertexArrays(1, &vao_);
    vboList_.resize(numOfAttibutePerVertex + 1);
    glCreateBuffers(vboList_.size(), vboList_.data());
    //glGenVertexArrays(1, &vao_);
    //glGenBuffers(vboList_.size(), vboList_.data());
    std::cerr << "Mesh Cons" << std::endl;
}

Shiny::Mesh::~Mesh()
{
    std::cerr << "Mesh Destroy" << std::endl;
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(vboList_.size(), vboList_.data());
}

void Shiny::Mesh::LoadVertexAttribute(int index, int numOfChannel, bool normalized, const std::vector<short>& data)
{
    //glBindVertexArray(vao_);
    //glEnableVertexAttribArray(index);
    //glBindBuffer(GL_ARRAY_BUFFER, vboList_[index]);
    //glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    //glVertexAttribPointer(index, numOfChannel, GL_SHORT, GL_TRUE, 0, nullptr);
    //glBindVertexArray(0);
    //return;
    if (index >= vboList_.size() - 1) {
        return;
    }
    glNamedBufferStorage(vboList_[index], sizeof(data[0]) * data.size(), data.data(), 0);
    //glNamedBufferData(vboList_[index], data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao_, index);
    glVertexArrayVertexBuffer(vao_, index, vboList_[index], 0, numOfChannel * sizeof(data[0]));
    if (normalized) {
        glVertexArrayAttribFormat(vao_, index, numOfChannel, GL_SHORT, GL_TRUE, 0);
    } else {
        glVertexArrayAttribIFormat(vao_, index, numOfChannel, GL_SHORT, 0);
    }
    glVertexArrayAttribBinding(vao_, index, index);

    //glBindVertexArray(vao_);
    //glEnableVertexAttribArray(index);
    //glBindBuffer(GL_ARRAY_BUFFER, vboList_[index]);
    //glVertexAttribPointer(index, numOfChannel, GL_SHORT, GL_TRUE, 0, nullptr);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
}

void Shiny::Mesh::LoadVertexAttribute(int index, int numOfChannel, const std::vector<float>& data)
{
    //glBindVertexArray(vao_);
    //glEnableVertexAttribArray(index);
    //glBindBuffer(GL_ARRAY_BUFFER, vboList_[index]);
    //glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    //glVertexAttribPointer(index, numOfChannel, GL_FLOAT, GL_FALSE, 0, nullptr);
    //glBindVertexArray(0);
    //return;
    if (index >= vboList_.size() - 1) {
        return;
    }
    glNamedBufferStorage(vboList_[index], sizeof(data[0]) * data.size(), data.data(), 0);
    //glNamedBufferData(vboList_[index], data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao_, index);
    glVertexArrayVertexBuffer(vao_, index, vboList_[index], 0, numOfChannel * sizeof(data[0]));
    glVertexArrayAttribFormat(vao_, index, numOfChannel, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao_, index, index);
}

void Shiny::Mesh::LoadIndices(const std::vector<unsigned short>& indices)
{
    indexIsUnsignedShort = true;
    numOfIndex_ = indices.size();
    LoadIndices(indices.data(), indices.size() * sizeof(indices[0]));
}

void Shiny::Mesh::LoadIndices(const std::vector<unsigned int>& indices)
{
    indexIsUnsignedShort = false;
    numOfIndex_ = indices.size();
    LoadIndices(indices.data(), indices.size() * sizeof(indices[0]));
}


struct VertexDescription
{
    int numOfChannel_;
    GLenum type_;
    bool normalized_;
    int size_;
    VertexDescription(int numOfChannel,
                      GLenum type,
                      bool normalized,
                      int size) : 
        numOfChannel_(numOfChannel), type_(type), normalized_(normalized), size_(size)
    {
    }
};
void Shiny::Mesh::LoadStandardPackage(const std::string& name)
{
    
    const std::string directory = "../../Resources/Model/";
    const std::string prefix = directory + name;
    Json::JsonObject json;
    Json::Parser parser(&json, ResourceManager::ReadFileToString(prefix + ".json"));
    std::cerr << parser.GetErrorMessage() << std::endl;
    bool hasSkeleton = json.GetValue("has_skeleton").AsBool();
    
    auto vertexInfo = json.GetValue("vertex").AsJsonObject();
    int vertexDataOffset = vertexInfo->GetValue("offset").AsInt();
    int vertexDataLength = vertexInfo->GetValue("length").AsInt();

    auto indexInfo = json.GetValue("index").AsJsonObject();
    int indexDataOffset = indexInfo->GetValue("offset").AsInt();
    int indexDataLength = indexInfo->GetValue("length").AsInt();
    numOfIndex_ = indexInfo->GetValue("count").AsInt();

    std::cerr << vertexDataOffset << "," << vertexDataLength << "," << indexDataOffset << "," << indexDataLength << "," << numOfIndex_ << std::endl;

    std::vector<char> vertexData(vertexDataLength);
    std::vector<char> indexData(indexDataLength);

    std::ifstream inputFileStream(prefix + ".bin", std::ios::binary);
    inputFileStream.read(vertexData.data(), vertexDataLength);
    inputFileStream.read(indexData.data(), indexDataLength);
    inputFileStream.close();

    std::vector<VertexDescription> vertexDescList = {
        { 3, GL_FLOAT, false, 3 * sizeof(float) }, // position: float3
        { 4, GL_INT_2_10_10_10_REV, true, sizeof(Int_2_10_10_10) }, // normal: int 2_10_10_10
        //{ 4, GL_INT_2_10_10_10_REV, true, sizeof(Int_2_10_10_10) }, // binormal: int 2_10_10_10
        { 2, GL_UNSIGNED_SHORT, true, sizeof(unsigned short) * 2 } // texcoord: unsigned short2
    };
    if (hasSkeleton) {
        vertexDescList.emplace_back(4, GL_UNSIGNED_SHORT, false, sizeof(unsigned short) * 4);
        vertexDescList.emplace_back(4, GL_SHORT, true, sizeof(short) * 4);
    }
    int stride = 0;// 40;
    for (const auto& desc : vertexDescList) {
        stride += desc.size_;
    }
    std::cerr << "stride" << stride << std::endl;
    glNamedBufferStorage(vboList_[0], vertexData.size(), vertexData.data(), 0);
    glVertexArrayVertexBuffer(vao_, 0, vboList_[0], 0, stride);
    for (int index = 0, relativeOffset = 0; index < vertexDescList.size(); index++) {
        const auto& desc = vertexDescList[index];
        glEnableVertexArrayAttrib(vao_, index);
        glVertexArrayAttribFormat(vao_, index, desc.numOfChannel_, desc.type_, desc.normalized_, relativeOffset);
        relativeOffset += desc.size_;
        glVertexArrayAttribBinding(vao_, index, 0);
        std::cerr << "relative: " << relativeOffset << std::endl;
    }
    int* d = (int*)indexData.data();
    std::cerr << vboList_ .size() << std::endl;
    glNamedBufferStorage(vboList_[1], indexData.size(), indexData.data(), 0);
    glVertexArrayElementBuffer(vao_, vboList_[1]);
}

void Shiny::Mesh::Render()
{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, numOfIndex_, indexIsUnsignedShort ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Shiny::Mesh::LoadIndices(const void* data, int size)
{
    //glBindVertexArray(vao_);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboList_.back());
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    //glBindVertexArray(0);
    //return;
    glNamedBufferStorage(vboList_.back(), size, data, 0);
    //glNamedBufferData(vboList_.back(), size, data, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao_, vboList_.back());
}
