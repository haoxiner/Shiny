#include "StaticShader.h"
#include <fstream>

bool Shiny::StaticShader::Startup() {
    std::ifstream file;
    file.open("Shader/depth.vert.glsl");
    std::istreambuf_iterator<char> vsBegin(file), vsEnd;
    std::string vertexShaderSource(vsBegin, vsEnd);
    file.close();
    file.clear();

    file.open("Shader/depth.frag.glsl");
    std::istreambuf_iterator<char> fsBegin(file), fsEnd;
    std::string fragmentShaderSource(fsBegin, fsEnd);
    file.close();
    file.clear();
    bool success = ShaderProgram::Startup(vertexShaderSource, fragmentShaderSource);
    if (!success) {
        return false;
    }
    //Begin();
    //LoadInteger(GetUniformLocation("albedoMap"), 0);
    //LoadInteger(GetUniformLocation("metallicMap"), 1);
    //LoadInteger(GetUniformLocation("roughnessMap"), 2);
    //LoadInteger(GetUniformLocation("normalMap"), 3);
    //End();
    return true;
}

void Shiny::StaticShader::BindAttributes() {
    BindAttributeLocation(0, "position");
    //BindAttributeLocation(1, "normal");
    //BindAttributeLocation(2, "texCoord");
}

void Shiny::StaticShader::GetAllUniformLocations() {
    modelMatrixLocation_ = GetUniformLocation("modelMatrix");
    viewMatrixLocation_ = GetUniformLocation("viewMatrix");
    projectionMatrixLocation_ = GetUniformLocation("projectionMatrix");
}

void Shiny::StaticShader::LoadModelMatrix(const Matrix4x4f& value) {
    LoadMatrix(modelMatrixLocation_, value);
}

//void Shiny::StaticShader::LoadViewMatrix(const Camera& camera) {
//    LoadMatrix(viewMatrixLocation_, camera.GetViewMatrix());
//}

void Shiny::StaticShader::LoadProjectionMatrix(const Matrix4x4f& value) {
    LoadMatrix(projectionMatrixLocation_, value);
}