//
//  MultiChain.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/9/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#include "MultiChain.h"

MultiChain::MultiChain(vector<Chain *> &chains) {
    /*构造方法*/
    root = new ChainNode();
    root->value = *chains.begin();
    root->parent = NULL;
    root->children = new vector<ChainNode *>();
    bool success = false;

    origin = root->value->origin;

    for (auto it = chains.begin() + 1; it != chains.end(); ++it) {
        Chain *current = *it;
        success = true && insert(root, current);
    }

    if (!success) {
        cerr << "Something went wrong when constructing the multichain; one of the nodes probably didn't match up."
             << endl;
        exit(-1);
    }
}

bool MultiChain::insert(ChainNode *root, Chain *chain) {
    if (glm::all(glm::equal(root->value->end, chain->origin))) {
        //该骨骼链与父节点尾部相接，为子节点
        ChainNode *new_node = new ChainNode();
        new_node->value = chain;
        new_node->parent = root;
        new_node->children = new vector<ChainNode *>();

        root->children->push_back(new_node);
        root->value->target = new Target(0, 0, 0);

        //标记叶子节点
        if (leaves[root]) leaves[root] = false;
        leaves[new_node] = true;

        return true;

    } else if (!root->children->empty()) {
        //非子节点
        bool was_inserted = false;
        for (auto it = root->children->begin(); it != root->children->end(); ++it) {
            ChainNode *recurse = *it;
            was_inserted = was_inserted || insert(recurse, chain);
            if (was_inserted) break;
        }
        return was_inserted;

    }

    return false; //当有某条链独立时，连接失败
}

void MultiChain::solve() {
    /*计算IK*/
    root->value->origin = origin;
    root->value->setFirstJoint(origin);

    backward(root);
    root->value->solve();
    forward(root);

}

void MultiChain::backward(ChainNode *root) {
    /*后向计算*/
    if (!root->children->empty()) {
        for (auto it = root->children->begin(); it != root->children->end(); ++it) {
            backward(*it);
        }

        //计算质心
        glm::vec3 centroid(0);
        int i = 0;
        for (auto it = root->children->begin(); it != root->children->end(); ++it) {
            ChainNode *node = *it;
            centroid += node->value->getFirstJoint();
            ++i;
        }
        centroid = centroid / (float) i;
        //if(glm::length(centroid - root->value->origin) < root->value->total_length)
        root->value->target->position = centroid;
    }

    if (root->parent) {
        root->value->backward();
    }
}

void MultiChain::forward(ChainNode *root) {
    /*前向计算*/
    if (!root->children->empty()) {
        for (auto it = root->children->begin(); it != root->children->end(); ++it) {
            forward(*it);
        }
    }

    if (root->parent) {
        root->value->origin = root->parent->value->end;
        root->value->forward();
        root->value->setSegments();
    }
}

void MultiChain::render(glm::mat4 &view, glm::mat4 &projection, glm::mat4 &lightSpaceMatrix, GLuint depthMap) {
    /*使用栈模拟递归绘制所有节点*/
    stack<ChainNode *> traverse;
    traverse.push(root);

    while (!traverse.empty()) {
        ChainNode *cur = traverse.top();

        cur->value->render(view, projection, lightSpaceMatrix, depthMap);
        traverse.pop();
        for (auto it = cur->children->begin(); it != cur->children->end(); ++it) {
            traverse.push(*it);
        }
    }

}

void MultiChain::moveHead(Camera_Movement direction, GLfloat deltaTime) {
    /*移动头结点*/
    GLfloat velocity = 2.0f * deltaTime;
    GLfloat x=origin[0],y=origin[1],z=origin[2];
/*    glm::vec3 shift(0);
    if (direction == UP)
        shift[1] += 1.0f * velocity;
    if (direction == DOWN)
        shift[1] -= 1.0f * velocity;
    if (direction == LEFT)
        shift[0] -= 1.0f * velocity;
    if (direction == RIGHT)
        shift[0] += 1.0f * velocity;
    if (direction == FORWARD)
        shift[2] += 1.0f * velocity;
    if (direction == BACKWARD)
        shift[2] -= 1.0f * velocity;
    origin+=shift;*/

    if (direction == UP)
        y += 1.0f * velocity;
    if (direction == DOWN)
        y -= 1.0f * velocity;
    if (direction == LEFT)
        x -= 1.0f * velocity;
    if (direction == RIGHT)
        x += 1.0f * velocity;
    if (direction == FORWARD)
        z += 1.0f * velocity;
    if (direction == BACKWARD)
        z -= 1.0f * velocity;
    origin=glm::vec3(x,y,z);
    //resetTarget(root,shift);
}

void MultiChain::resetTarget(ChainNode *root,glm::vec3 shift) {
    root->value->target->position=root->value->end;
    if (!root->children->empty()) {
        for (auto it = root->children->begin(); it != root->children->end(); ++it) {
            resetTarget(*it,shift);
        }
    }
}

void MultiChain::renderDepthMap(glm::mat4 &lightSpaceMatrix) {
    /*使用栈模拟递归绘制所有节点*/
    stack<ChainNode *> traverse;
    traverse.push(root);

    while (!traverse.empty()) {
        ChainNode *cur = traverse.top();

        cur->value->renderDepthMap(lightSpaceMatrix);
        traverse.pop();
        for (auto it = cur->children->begin(); it != cur->children->end(); ++it) {
            traverse.push(*it);
        }
    }
}


