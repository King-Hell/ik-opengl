//
// Created by kinghell on 19-5-27.
//
#pragma once

#include "Shader.h"
#include <stb/stb_image.h>
class Ground {
public:
    Ground();
    void render(glm::mat4 &view, glm::mat4 &projection, glm::mat4 &lightSpaceMatrix, GLuint depthMap);
    void renderDepthMap(glm::mat4 &lightSpaceMatrix);
private:
    void loadVertex();
    unsigned int loadTexture(char const * path);
    unsigned int planeVAO;
    unsigned int woodTexture;
    Shader shader;
    Shader simpleDepthShader;
};


