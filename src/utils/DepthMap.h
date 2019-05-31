//
// Created by kinghell on 19-5-28.
//

#pragma once

#include "Shader.h"
class DepthMap {
    /*深度贴图类*/
public:
    DepthMap(GLuint shadow_width=1024,GLuint shadow_height=1024);
    void begin();
    void end(GLuint scr_width,GLuint scr_height);

    GLuint getDepthMap(){return depthMap;}
private:
    Shader shader;
    GLuint depthMapFBO,depthMap;
    GLuint shadow_width,shadow_height;
};

