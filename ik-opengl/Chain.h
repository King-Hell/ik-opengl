//
//  Chain.h
//  ik-opengl
//
//  Created by Jesse Zhou on 6/2/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//
//骨骼链类
#pragma once

#include <stdio.h>
#include <vector>

#include "Target.h"
#include "Segment.h"

// GLM includes
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/projection.hpp>
#include <glad/glad.h>

const glm::vec3 ref_rot_vector(0.0f, 0.0f, -1.0f);

class Chain {
public:
    Chain(vector<glm::vec3> joints, Target *t);

    Chain(glm::vec3 origin, glm::vec3 end, Target *t, int partitions = 5);

    void Render(glm::mat4 view, glm::mat4 proj);

    void Solve();

    void Backward(); // Put second endpoint at target and work backwards
    void Forward();  // Put first endpoint at origin and work forwards
    glm::vec3 Constrain(glm::vec3 point, float true_length, Segment *seg);

    void CalculateLinks(vector<glm::vec3> joints, vector<float> *lengths, vector<glm::quat> *directions);

    glm::vec3 GetFirstJoint();

    void SetFirstJoint(glm::vec3 joint);

    void SetSegments();

    unsigned long size;//关节数量
    float total_length;//骨骼链总长度
    glm::vec3 origin;//头关节
    glm::vec3 end;//尾关节
    Target *target;
    bool please_constrain = false;

private:
    vector<glm::vec3> joints; // 关节
    vector<Segment> segments; // 骨骼
    float tolerance = 0.01f;

};
