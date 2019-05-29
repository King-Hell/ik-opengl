//
//  Segment.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/1/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#include "Segment.h"

Segment::Segment(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir, GLfloat xScale, GLfloat yScale) {

    // Shader init
    shader = Shader(vertexShaderPath, fragShaderPath);
    simpleDepthShader = Shader(vertexShadowShaderPath, fragShadowShaderPath);
    this->xScale = xScale;
    this->yScale = yScale;
    hasTexture = false;
    renderInit();
    loadCubemap("res/textures/snowhite.jpg", 1);
    set(base, end, magnitude, dir);
}

void Segment::set(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir) {
    quat = dir;
    position = base;
    end_position = end;
    this->magnitude = magnitude;
    this->constraint_cone = glm::vec4(45.0f, 45.0f, 45.0f, 45.0f);
}

void Segment::render(glm::mat4 &view, glm::mat4 &projection, glm::mat4 &lightSpaceMatrix, GLuint depthMap) {
    /*渲染函数*/
    //计算模型矩阵 位移 缩放 旋转
    glm::mat4 model;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 PT = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.5f));
    glm::mat4 PS = glm::scale(glm::mat4(1.0f), glm::vec3(xScale, yScale, 1.0f));
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, magnitude));
    glm::mat4 R = glm::toMat4(quat);
    model = T * R * S * PT * PS;
    //渲染物体
    shader.use();
    //设置片段着色器 物体颜色 光照颜色 光照位置 视角位置
    //shader.setFloat3("objectColor", 1.0f, 1.0f, 1.0f);
    shader.setFloat3("lightColor", 1.0f, 1.0f, 1.0f);
    shader.setFloat3("lightPos", -3.0f, 3.0f, 3.0f);
    shader.setFloat3("viewPos", 0, 0, 3.0f);
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    shader.setFloat1("material.shininess", 16.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}

void Segment::loadCubemap(string path, GLuint mapNum) {
    /*加载纹理贴图*/
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

    int width, height, nrChannels;
    if(mapNum==1){
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            for (int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                             data);
            }
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }
    }else{
        string pathBase=path.substr(0,path.find("."));
        for(int i=0;i<mapNum;i++){
            char pathEnd[7];
            sprintf(pathEnd,"_%d.png",i);
            string newPath=pathBase+pathEnd;
            unsigned char *data = stbi_load(newPath.c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,data);
                stbi_image_free(data);
            } else {
                std::cout << "Cubemap texture failed to load at path: " << newPath << std::endl;
                stbi_image_free(data);
            }
        }
    }
    cubemapTexture = textureID;
}

glm::mat4 Segment::GetFaceNormals() {
//  
//  glm::vec3 up = glm::vec3(0, 1, 0);
//  glm::vec3 right = glm::cross(glm::vec3(0, 0, -1), up);

//  glm::vec3 up_relative = up * glm::mat3(glm::toMat4(quat));
//  glm::vec3 right_relative = right * glm::mat3(glm::toMat4(quat));
//  glm::vec3 left_relative = -1.0f * right_relative;
//  glm::vec3 down_relative = -1.0f * up_relative;

    glm::vec3 up_relative = glm::cross(glm::vec3(0, 0, -1), glm::normalize(end_position - position));
    glm::vec3 right_relative = glm::cross(glm::vec3(0, 0, -1), up_relative);
    glm::vec3 left_relative = -1.0f * right_relative;
    glm::vec3 down_relative = -1.0f * up_relative;


    return glm::mat4(
            glm::vec4(glm::normalize(up_relative), 0.0f),
            glm::vec4(glm::normalize(down_relative), 0.0f),
            glm::vec4(glm::normalize(left_relative), 0.0f),
            glm::vec4(glm::normalize(right_relative), 0.0f)
    );

}

glm::vec3 Segment::GetConstraintConeAxis() {
    //glm::vec3 direction = glm::vec3(0, 0, -1) * glm::mat3(glm::toMat4(quat));
    return end_position - position;
}

void Segment::SetConstraintConeDegrees(glm::vec4 degrees) {
    constraint_cone = degrees;
}

void Segment::renderInit() {
    // VBO 六面体
    GLfloat vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    shader.use();
    shader.setInt("cubeTexture", 0);
    shader.setInt("shadowMap", 1);
}

Segment::~Segment() {
    //glDeleteVertexArrays(1,&cubeVAO);
    //glDeleteBuffers(1,&VBO);
}

void Segment::renderDepthMap(glm::mat4 &lightSpaceMatrix) {
    /*渲染深度贴图*/
    //计算模型矩阵 位移 缩放 旋转
    glm::mat4 model;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 PT = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.5f));
    glm::mat4 PS = glm::scale(glm::mat4(1.0f), glm::vec3(xScale, yScale, 1.0f));
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, magnitude));
    glm::mat4 R = glm::toMat4(quat);
    model = T * R * S * PT * PS;
    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    simpleDepthShader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
