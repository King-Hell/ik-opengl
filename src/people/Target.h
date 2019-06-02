//目标点类
#pragma once

#include "../utils/Shader.h"
#include "../utils/Model.h"
#include "../utils/Camera.h"

// Std includes
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glad/glad.h>

class Target {
public:
    /* Data */
    glm::vec3 position;//目标点位置
    float pitch;//俯仰角
    float yaw;//偏航角
    glm::vec3 scale;//缩放

    /* Functions */
    Target(){};

    Target(glm::vec3 pos);//构造方法
    void render(glm::mat4 &view, glm::mat4 &projection);//渲染方法
    void processTranslation(Camera_Movement direction, GLfloat deltaTime);//位移方法

private:

    /* Data */
    Model objectModel;//模型
    const GLchar *pathToModel = "res/models/sphere.off";//模型路径
    const GLchar *vertexShaderPath = "res/shaders/shader.vs";//顶点着色器
    const GLchar *fragShaderPath = "res/shaders/shader.fs";//片段着色器
    Shader objectShader;//着色器对象

    /* Functions */
};
