//
//  MultiChain.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/9/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#include "MultiChain.h"

MultiChain::MultiChain(vector<Chain *> chains) {
    /*构造方法*/
    root = new ChainNode();
    root->value = *chains.begin();
    root->parent = NULL;
    root->children = new vector<ChainNode *>();
    bool success = false;

    origin = root->value->origin;

    for (auto it = chains.begin() + 1; it != chains.end(); ++it) {
        Chain *current = *it;
        success = true && Insert(root, current);
    }

    if (!success) {
        cerr << "Something went wrong when constructing the multichain; one of the nodes probably didn't match up."
             << endl;
        exit(-1);
    }
}

bool MultiChain::Insert(ChainNode *root, Chain *chain) {
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
            was_inserted = was_inserted || Insert(recurse, chain);
            if (was_inserted) break;
        }
        return was_inserted;

    }

    return false; //当有某条链独立时，连接失败
}

void MultiChain::Solve() {
    /*计算IK*/
    root->value->origin = origin;
    root->value->SetFirstJoint(origin);

    Backward(root);
    root->value->Solve();
    Forward(root);

}

void MultiChain::Backward(ChainNode *root) {
    /*后向计算*/
    if (!root->children->empty()) {
        for (auto it = root->children->begin(); it != root->children->end(); ++it) {
            Backward(*it);
        }

        //计算质心
        glm::vec3 centroid;
        int i = 0;
        for (auto it = root->children->begin(); it != root->children->end(); ++it) {
            ChainNode *node = *it;
            centroid += node->value->GetFirstJoint();
            ++i;
        }
        centroid = centroid / (float) i;
        //if(glm::length(centroid - root->value->origin) < root->value->total_length)
        root->value->target->position = centroid;
    }

    if (root->parent) {
        root->value->Backward();
    }
}

void MultiChain::Forward(ChainNode *root) {
    /*前向计算*/
    if (!root->children->empty()) {
        for (auto it = root->children->begin(); it != root->children->end(); ++it) {
            Forward(*it);
        }
    }

    if (root->parent) {
        root->value->origin = root->parent->value->end;
        root->value->Forward();
        root->value->SetSegments();
    }
}

void MultiChain::Render(glm::mat4 view, glm::mat4 proj) {
    /*使用栈模拟递归绘制所有节点*/
    stack<ChainNode *> traverse;
    traverse.push(root);

    while (!traverse.empty()) {
        ChainNode *cur = traverse.top();

        cur->value->Render(view, proj);
        traverse.pop();
        for (auto it = cur->children->begin(); it != cur->children->end(); ++it) {
            traverse.push(*it);
        }
    }

}
