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

Segment::Segment(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir) {

    // Shader init
    Shader modelS(vertexShaderPath, fragShaderPath);
    objectShader = modelS;

    Set(base, end, magnitude, dir);
}

void Segment::Set(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir) {
    quat = dir;
    position = base;
    end_position = end;
    this->magnitude = magnitude;
    this->constraint_cone = glm::vec4(45.0f, 45.0f, 45.0f, 45.0f);
}

void Segment::Render(glm::mat4 view, glm::mat4 proj) {
    /*渲染函数*/
    objectShader.Use();
    //设置片段着色器 物体颜色 光照颜色 光照位置 视角位置
    objectShader.setFloat3("objectColor", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat3("lightColor", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat3("lightPos", 1.0f, 1.0f, 3.0f);
    objectShader.setFloat3("viewPos", 0.0, 0.0, 3.0);
    //设置顶点着色器 位移 缩放 旋转
    glm::mat4 model;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 PT = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.5));
    glm::mat4 PS = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 1.0f));
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, magnitude));
    glm::mat4 R = glm::toMat4(quat);
    model = T * R * S * PT * PS;
    objectShader.setMat4("model",model);
    objectShader.setMat4("view",view);
    objectShader.setMat4("projection",proj);
    // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(glGetUniformLocation(objectShader.Program, "material.shininess"), 16.0f);

    // VBO stuff

    float vertices[] = {
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
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
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


    glDrawArrays(GL_TRIANGLES, 0, 36);

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
