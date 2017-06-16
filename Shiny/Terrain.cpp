#include "Terrain.h"
#include "Vertex.h"
#include <vector>

Shiny::Terrain::Terrain(const std::string& name)
{
    numOfVertexPerLine_ = 256;
    width_ = 128;

    const float heightScale = 3.0f;
    std::vector<Vertex> vertices;
    for (int yIdx = 0; yIdx < numOfVertexPerLine_; yIdx++) {
        for (int xIdx = 0; xIdx < numOfVertexPerLine_; xIdx++) {
            Vertex vertex;
            vertex.p.x = xIdx / ((float)(numOfVertexPerLine_ - 1)) * width_;
            vertex.p.y = yIdx / ((float)(numOfVertexPerLine_ - 1)) * width_;
            vertex.p.z = 0.0f;
            vertex.n = PackFloat3ToInt2_10_10_10({ 0,0,1 });
            vertex.tx = xIdx / ((float)(numOfVertexPerLine_ - 1));
            vertex.ty = yIdx / ((float)(numOfVertexPerLine_ - 1));
            vertices.emplace_back(vertex);
        }
    }

    std::vector<unsigned int> indices;
    for (int yIdx = 0; yIdx < numOfVertexPerLine_ - 1; yIdx++) {
        for (int xIdx = 0; xIdx < numOfVertexPerLine_ - 1; xIdx++) {
            int bottomLeft = (yIdx * numOfVertexPerLine_) + xIdx;
            int bottomRight = bottomLeft + 1;
            int topLeft = bottomLeft + numOfVertexPerLine_;
            int topRight = topLeft + 1;

            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topLeft);
        }
    }
    mesh_.reset(new Mesh(1));
    mesh_->LoadVertices(vertices.data(), vertices.size() * sizeof(vertices[0]), {
        { 3, GL_FLOAT, false, 3 * sizeof(float) }, // position: float3
        { 4, GL_INT_2_10_10_10_REV, true, sizeof(Int_2_10_10_10) }, // normal: int 2_10_10_10
        //{ 4, GL_INT_2_10_10_10_REV, true, sizeof(Int_2_10_10_10) }, // binormal: int 2_10_10_10
        { 2, GL_UNSIGNED_SHORT, true, sizeof(unsigned short) * 2 } // texcoord: unsigned short2
    });
    mesh_->LoadIndices(indices);
    material_.reset(new Material("bronze_copper"));
}

void Shiny::Terrain::Render() const
{
    material_->Use();
    mesh_->Render();
}
