#include "SkyBox.h"
#include "MathUtil.h"
#include "FreeImage.h"

Shiny::SkyBox::SkyBox() :mesh_(1)
{
    //std::vector<short> positions = {
    //    // bottom
    //    MapToShort(-1.0f), MapToShort(-1.0f), MapToShort(-1.0f), MapToShort(0.0f), // top left
    //    MapToShort( 1.0f), MapToShort(-1.0f), MapToShort(-1.0f), MapToShort(0.0f), // top right
    //    MapToShort(-1.0f), MapToShort(-1.0f), MapToShort( 1.0f), MapToShort(0.0f), // bottom left
    //    MapToShort( 1.0f), MapToShort(-1.0f), MapToShort( 1.0f), MapToShort(0.0f), // bottom right
    //    // top
    //    MapToShort(-1.0f), MapToShort(1.0f), MapToShort(-1.0f), MapToShort(0.0f), // top left
    //    MapToShort( 1.0f), MapToShort(1.0f), MapToShort(-1.0f), MapToShort(0.0f), // top right
    //    MapToShort(-1.0f), MapToShort(1.0f), MapToShort( 1.0f), MapToShort(0.0f), // bottom left
    //    MapToShort( 1.0f), MapToShort(1.0f), MapToShort( 1.0f), MapToShort(0.0f) // bottom right
    //};
    //std::vector<unsigned short> indices = {
    //    0,2,1, 1,2,3, // bottom
    //    6,4,7, 7,4,5, // top
    //    6,2,4, 4,2,0, // left
    //    5,1,7, 7,1,3, // right
    //    4,0,5, 5,0,1, // front
    //    6,2,7, 7,2,3 // back
    //};
    std::vector<short> positions = {
        MapToShort(1.0f), MapToShort(1.0f), MapToShort(0.0f), MapToShort(1.0f),
        MapToShort(-1.0f), MapToShort(1.0f), MapToShort(0.0f), MapToShort(1.0f),
        MapToShort(-1.0f), MapToShort(-1.0f), MapToShort(0.0f), MapToShort(1.0f),
        MapToShort(1.0f), MapToShort(-1.0f), MapToShort(0.0f), MapToShort(1.0f)
    };
    std::vector<unsigned int> indices = { 0,1,2, 0,2,3 };
    mesh_.LoadVertexAttribute(0, 4, true, positions);
    mesh_.LoadIndices(indices);

    unsigned int w(0), h(0);
    FIBITMAP* dib = FreeImage_Load(FIF_EXR, "../../envmap.exr");
    auto colorType = FreeImage_GetColorType(dib);
    auto bpp = FreeImage_GetBPP(dib);
    w = FreeImage_GetWidth(dib);
    h = FreeImage_GetHeight(dib);
    auto bits = FreeImage_GetBits(dib);
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID_);
    glTextureStorage2D(textureID_, 1, GL_RGB16F, w, h);
    glTextureSubImage2D(textureID_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
    FreeImage_Unload(dib);
    glBindTextureUnit(0, textureID_);
}
