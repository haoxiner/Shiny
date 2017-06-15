#include "SkyBox.h"
#include "MathUtil.h"
#include "FreeImage.h"
#include <iostream>
std::unique_ptr<Shiny::Mesh> Shiny::SkyBox::mesh_;

Shiny::SkyBox::SkyBox(const std::string& name)
{
    if (!mesh_.get()) {
        std::vector<short> positions = {
            // bottom
            MapToShort(-1.0f), MapToShort(1.0f), MapToShort(-1.0f), MapToShort(0.0f), // top left 0
            MapToShort(1.0f),  MapToShort(1.0f), MapToShort(-1.0f), MapToShort(0.0f), // top right 1
            MapToShort(-1.0f), MapToShort(-1.0f),  MapToShort(-1.0f), MapToShort(0.0f), // bottom left 2
            MapToShort(1.0f),  MapToShort(-1.0f),  MapToShort(-1.0f), MapToShort(0.0f), // bottom right 3
            // top
            MapToShort(-1.0f), MapToShort(1.0f), MapToShort(1.0f), MapToShort(0.0f), // top left 4
            MapToShort(1.0f),  MapToShort(1.0f), MapToShort(1.0f), MapToShort(0.0f), // top right 5
            MapToShort(-1.0f), MapToShort(-1.0f),  MapToShort(1.0f), MapToShort(0.0f), // bottom left 6
            MapToShort(1.0f),  MapToShort(-1.0f),  MapToShort(1.0f), MapToShort(0.0f) // bottom right 7
        };
        std::vector<unsigned short> indices = {
            0,2,1, 1,2,3, // bottom
            6,4,7, 7,4,5, // top
            6,2,4, 4,2,0, // left
            5,1,7, 7,1,3, // right
            4,0,5, 5,0,1, // front
            6,7,2, 7,3,2 // back
        };
        mesh_.reset(new Mesh(1));
        mesh_->LoadVertexAttribute(0, 4, true, positions);
        mesh_->LoadIndices(indices);
    }
    cubemap_.reset(new Cubemap("../../Resources/sky/" + name, name, false));
}

void Shiny::SkyBox::Render()
{
    cubemap_->BindTextureUint(0);
    mesh_->Render();
}
