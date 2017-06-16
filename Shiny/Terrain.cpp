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
        }
    }

    std::vector<int> indices;
    for (int yIdx = 0; yIdx < numOfVertexPerLine_; yIdx++) {
        for (int xIdx = 0; xIdx < numOfVertexPerLine_; xIdx++) {
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
}
