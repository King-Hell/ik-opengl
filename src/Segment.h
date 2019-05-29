//
//  Segment.hpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/1/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//
#pragma once
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include <stb/stb_image.h>
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

class Segment {
public:
    glm::vec3 position;
    glm::vec3 end_position;
    glm::quat quat;
    float magnitude;

    // The constraint cone, symbolized by the degrees going in the up, down, left, right directions
    glm::vec4 constraint_cone;

    void SetConstraintConeDegrees(glm::vec4 degrees);

    // Functions
    Segment(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir,GLfloat xScale=0.1f,GLfloat yScale=0.1f);

    ~Segment();

    void render(glm::mat4 &view, glm::mat4 &projection, glm::mat4 &lightSpaceMatrix, GLuint depthMap);

    void renderDepthMap(glm::mat4 &lightSpaceMatrix);

    void processTranslation(Camera_Movement direction, GLfloat deltaTime);

    void set(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir);

    // 0, 1, 2, 3 = Up, Down, Left, Right. Make sure you wrap each index around a vec3
    glm::mat4 GetFaceNormals();

    glm::vec3 GetConstraintConeAxis();

    void loadCubemap(string path, GLuint mapNum=1);

private:

    /* Data */
    const GLchar *vertexShaderPath = "res/shaders/seg.vs";
    const GLchar *fragShaderPath = "res/shaders/seg.fs";
    const GLchar *vertexShadowShaderPath="res/shaders/shadow.vs";
    const GLchar *fragShadowShaderPath = "res/shaders/shadow.fs";
    Shader shader,simpleDepthShader;
    GLfloat xScale;
    GLfloat yScale;
    bool hasTexture;
    GLuint VBO, cubeVAO,cubemapTexture;

    /* Functions */
    void renderWithTexture(glm::mat4 &view, glm::mat4 &proj);
    void renderInit();
};
