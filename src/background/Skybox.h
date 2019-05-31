//
// Created by kinghell on 19-5-8.
//

#pragma once
#include <stb/stb_image.h>
#include "../utils/Shader.h"

//Std
#include <vector>
class Skybox {
public:
    /*Functions*/
    Skybox();
    void render(glm::mat4 &view, glm::mat4 &proj);
    unsigned int loadCubemap(std::vector<std::string> faces);
private:
    /*Data*/
    const GLchar *vertexShaderPath = "res/shaders/skybox.vs";//顶点着色器
    const GLchar *fragShaderPath = "res/shaders/skybox.fs";//片段着色器
    Shader shader;//着色器对象
    unsigned int skyboxVBO;
    unsigned int skyboxVAO;
    unsigned int cubemapTexture;
};


