//
//  MultiChain.hpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/9/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <vector>
#include <stack>
#include <map>

#include "Chain.h"
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
#include <glm/gtx/projection.hpp>
#include <glad/glad.h>

// Tree data structure
struct ChainNode {
  Chain * value;
  ChainNode * parent;
  vector<ChainNode*> * children;
};

class MultiChain {

public:
  MultiChain(vector<Chain*> chains);
  bool insert(ChainNode *root, Chain *chain);
  void solve();
  void render(glm::mat4 &view, glm::mat4 &projection, glm::mat4 &lightSpaceMatrix, GLuint depthMap);
  void renderDepthMap(glm::mat4 &lightSpaceMatrix);
  void moveHead(Camera_Movement direction, GLfloat deltaTime);

  ChainNode * root;//根节点
  map<ChainNode*, bool> leaves;
  glm::vec3 origin;
  
private:
  void forward(ChainNode *root);
  void backward(ChainNode *root);
  void resetTarget(ChainNode *root,glm::vec3 shift);
};
