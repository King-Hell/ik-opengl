//
// Created by kinghell on 19-5-28.
//

#include "DepthMap.h"

DepthMap::DepthMap(GLuint shadow_width,GLuint shadow_height) {
    /*构造方法*/
    this->shadow_width=shadow_width;
    this->shadow_height=shadow_height;
    glGenFramebuffers(1, &depthMapFBO);
    //创建深度贴图
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void DepthMap::begin() {
    /*渲染深度贴图前置过程*/
    glViewport(0,0,shadow_width,shadow_height);
    glBindFramebuffer(GL_FRAMEBUFFER,depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void DepthMap::end(GLuint scr_width,GLuint scr_height) {
    /*渲染深度贴图前后置过程*/
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,scr_width,scr_height);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

