#include "ResourceManager.h"
#include "TinyObjLoader.h"
#include "MathUtil.h"
#include <vector>
#include <iostream>
#include <memory>

std::string Shiny::ResourceManager::ReadFileToString(const std::string& fileName)
{
    std::ifstream fileInputStream(fileName);
    if (!fileInputStream) {
        return "";
    }
    std::istreambuf_iterator<char> fileBegin(fileInputStream), fileEnd;
    return std::string(fileBegin, fileEnd);
}

bool Shiny::ResourceManager::LoadObjToMesh(const std::string& fileName, Mesh& mesh)
{
    tinyobj::attrib_t attrib;
    std::string err;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str());
    if (!ret) {
        return false;
    }
    std::vector<float> vertexAttribute0;
    std::vector<float> vertexAttribute1;
    std::vector<unsigned int> indices;
    for (size_t s = 0; s < 1; s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++) {
                auto idx = shapes[s].mesh.indices[index_offset + v];
                auto vx = attrib.vertices[3 * idx.vertex_index + 0];
                auto vy = attrib.vertices[3 * idx.vertex_index + 1];
                auto vz = attrib.vertices[3 * idx.vertex_index + 2];
                auto nx = attrib.normals[3 * idx.normal_index + 0];
                auto ny = attrib.normals[3 * idx.normal_index + 1];
                auto nz = attrib.normals[3 * idx.normal_index + 2];
                auto tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                auto ty = attrib.texcoords[2 * idx.texcoord_index + 1];

                indices.push_back(vertexAttribute0.size() / 4);
                vertexAttribute0.push_back((vx));
                vertexAttribute0.push_back((vy));
                vertexAttribute0.push_back((vz));
                vertexAttribute0.push_back((tx));

                vertexAttribute1.push_back((nx));
                vertexAttribute1.push_back((ny));
                vertexAttribute1.push_back((nz));
                vertexAttribute1.push_back((ty));
            }
            index_offset += fv;
        }
    }
    std::cerr << vertexAttribute0.size()/4 << std::endl;
    mesh.LoadIndices(indices);
    mesh.LoadVertexAttribute(0, 4, vertexAttribute0);
    mesh.LoadVertexAttribute(1, 4, vertexAttribute1);
    return true;
}

namespace Shiny
{
struct Vertex
{
    float px;
    float py;
    float pz;
    Int_2_10_10_10 n;
    unsigned short tx;
    unsigned short ty;
};
struct SkinnedVertex
{
    float px;
    float py;
    float pz;
    Int_2_10_10_10 n;
    unsigned short tx;
    unsigned short ty;
    unsigned short bone[4];
    unsigned short weight[4];
    bool operator<(const SkinnedVertex& rhs) const
    {
        return std::memcmp((void*)this, (void*)&rhs, sizeof(SkinnedVertex)) < 0;
    };
};
}

void Shiny::ResourceManager::WriteObjToSPK(const std::string& objFileName, const std::string& spkFileName)
{
    tinyobj::attrib_t attrib;
    std::string err;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFileName.c_str());
    
    int numOfVertices = 23297;
    std::cerr << "NUM OF VERTICES: " << attrib.vertices.size() / 3 << std::endl;
    std::ifstream skin("../../Resources/Model/prototype.skin", std::ios::binary);

    std::vector<int> boneIDList(numOfVertices * 4);
    std::vector<float> boneWeightList(numOfVertices * 4);
    skin.read((char*)boneIDList.data(), sizeof(int)*boneIDList.size());
    skin.read((char*)boneWeightList.data(), sizeof(float)*boneWeightList.size());
    skin.close();

    // combine equal vertices
    std::map<SkinnedVertex, int> vertexMap;
    std::vector<SkinnedVertex> outputVertices;

    std::vector<int> indices;
    std::ofstream output(spkFileName, std::ios::binary);
    int offset = 0;
    for (size_t s = 0; s < 1; s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++) {
                auto idx = shapes[s].mesh.indices[index_offset + v];
                float vx = attrib.vertices[3 * idx.vertex_index + 0];
                float vy = attrib.vertices[3 * idx.vertex_index + 1];
                float vz = attrib.vertices[3 * idx.vertex_index + 2];
                float nx = attrib.normals[3 * idx.normal_index + 0];
                float ny = attrib.normals[3 * idx.normal_index + 1];
                float nz = attrib.normals[3 * idx.normal_index + 2];
                float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                float ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                SkinnedVertex vertex = {
                    vx,vy,vz,
                    PackFloat3ToInt2_10_10_10({ nx,ny,nz }),
                    MapToUnsignedShort(tx), MapToUnsignedShort(ty)
                };
                for (int i = 0; i < 4; i++) {
                    vertex.bone[i] = static_cast<unsigned short>(boneIDList[4 * idx.vertex_index + i]);
                }
                for (int i = 0; i < 4; i++) {
                    vertex.weight[i] = MapToUnsignedShort(boneWeightList[4 * idx.vertex_index + i]);
                }
                auto vMapIter = vertexMap.find(vertex);
                if (vMapIter == vertexMap.end()) {
                    int index = static_cast<int>(outputVertices.size());
                    indices.emplace_back(index);
                    vertexMap[vertex] = index;
                    outputVertices.emplace_back(vertex);
                } else {
                    indices.emplace_back(vMapIter->second);
                }
            }
            index_offset += fv;
        }
    }
    output.write((char*)outputVertices.data(), sizeof(SkinnedVertex) * outputVertices.size());
    offset += sizeof(SkinnedVertex) * outputVertices.size();
    std::cerr << "length/offset: " << offset << std::endl;
    output.write((char*)indices.data(), sizeof(int)*indices.size());
    int start = offset;
    offset += sizeof(int) * indices.size();
    std::cerr << "length: " << (offset - start) << std::endl;
    std::cerr << indices.size() << std::endl;
    output.close();
}
