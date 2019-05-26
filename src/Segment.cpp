//
//  Segment.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/1/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//


#include "Segment.h"

#include "Target.h"
#include "Camera.h"

Segment::Segment(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir, GLfloat xScale, GLfloat yScale) {

    // Shader init
    Shader modelS(vertexShaderPath, fragShaderPath);
    objectShader = modelS;
    this->xScale = xScale;
    this->yScale = yScale;
    hasTexture = false;
    renderInit();
    cubemapTexture = loadCubemap("res/textures/skin1.jpg");
    set(base, end, magnitude, dir);

}

void Segment::set(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir) {
    quat = dir;
    position = base;
    end_position = end;
    this->magnitude = magnitude;
    this->constraint_cone = glm::vec4(45.0f, 45.0f, 45.0f, 45.0f);
}

void Segment::render(glm::mat4 &view, glm::mat4 &proj) {
    /*渲染函数*/
    objectShader.use();
    //设置片段着色器 物体颜色 光照颜色 光照位置 视角位置
    objectShader.setFloat3("objectColor", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat3("lightColor", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat3("lightPos", 1.0f, 1.0f, 3.0f);
    objectShader.setFloat3("viewPos", 0.0, 0.0, 3.0);

    //loadCubemap("skin.jpg");
    //设置顶点着色器 位移 缩放 旋转
    glm::mat4 model;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 PT = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.5));
    glm::mat4 PS = glm::scale(glm::mat4(1.0f), glm::vec3(xScale, yScale, 1.0f));
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, magnitude));
    glm::mat4 R = glm::toMat4(quat);
    model = T * R * S * PT * PS;
    objectShader.setMat4("model", model);
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", proj);
    // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    objectShader.setFloat1("material.shininess", 16.0f);

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

unsigned int Segment::loadCubemap(string path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

    int width, height, nrChannels;
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

    return textureID;
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
    objectShader.use();
    objectShader.setInt("cubeTexture", 0);
}

Segment::~Segment() {
    //glDeleteVertexArrays(1,&cubeVAO);
    //glDeleteBuffers(1,&VBO);
}
