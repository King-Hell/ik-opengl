//
// Created by kinghell on 19-5-31.
//

#pragma once

#include "Target.h"
#include "Chain.h"
#include "MultiChain.h"

class Person {
public:
    Person();

    Target &getLeftArm() { return leftArm; }

    Target &getRightArm() { return rightArm; }

    Target &getLeftLeg() { return leftLeg; }

    Target &getRightLeg() { return rightLeg; }

    Target &getHead() { return head; }

    Target &getBody() { return body; }

    void solve() { multichain.solve(); }

    void render(glm::mat4 &view, glm::mat4 &projection, glm::mat4 &lightSpaceMatrix, GLuint depthMap) {
        multichain.render(view, projection, lightSpaceMatrix, depthMap);
    }

    void renderDepthMap(glm::mat4 &lightSpaceMatrix) { multichain.renderDepthMap(lightSpaceMatrix); }

    void moveHead(Camera_Movement direction, GLfloat deltaTime) { multichain.moveHead(direction, deltaTime); }

    void select(Target &selected) { this->selected = &selected; }

    void processTranslation(Camera_Movement direction, GLfloat deltaTime) {
        selected->processTranslation(direction, deltaTime);
    }

    void renderTarget(glm::mat4 &view, glm::mat4 &projection);

private:
    MultiChain multichain;
    Target leftArm, rightArm, leftLeg, rightLeg, head, body;
    Target *selected;
};
