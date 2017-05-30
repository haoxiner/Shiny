#include "ResourceManager.h"
#include "TinyObjLoader.h"
#include "MathUtil.h"
#include <vector>
#include <iostream>

std::string Shiny::ResourceManager::ReadFileToString(const std::string& path)
{
    std::ifstream fileInputStream(path);
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
