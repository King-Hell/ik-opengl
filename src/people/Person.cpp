//
// Created by kinghell on 19-5-31.
//

#include "Person.h"

Person::Person() {
    //加载模型
    leftArm=Target(-0.8f, 0, 0);
    rightArm=Target(0.8f, 0, 0);
    leftLeg=Target(-0.6f, -1.5f, 0);
    rightLeg=Target(0.6f, -1.5f, 0);
    head=Target(0, 0.5f, 0);
    body=Target(0, -0.5f, 0);
    vector<Chain *> vec;
    vector<glm::vec3> headJoints;
    headJoints.emplace_back(0, 0.5f, 0);
    headJoints.emplace_back(0, 0.1f, 0);
    headJoints.emplace_back(0, 0, 0);
    vector<glm::vec2> headScales;
    headScales.emplace_back(0.4f, 0.4f);
    headScales.emplace_back(0.1f, 0.1f);
    Chain *headNeck = new Chain(headJoints, &head, headScales);
    headNeck->setTexture(0, "res/textures/ironman.png");
    vec.push_back(headNeck);
    vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(-0.8f, 0, 0), &leftArm, 2));
    vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(0.8f, 0, 0), &rightArm, 2));
    vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(0, -0.7f, 0), &body, 1, 0.4f, 0.15f));
    vec.push_back(new Chain(glm::vec3(0, -0.7f, 0), glm::vec3(-0.6f, -1.5f, 0), &leftLeg, 2, 0.12f, 0.12f));
    vec.push_back(new Chain(glm::vec3(0, -0.7f, 0), glm::vec3(0.6f, -1.5f, 0), &rightLeg, 2, 0.12f, 0.12f));
    selected=&leftArm;
    multichain=MultiChain(vec);
}

void Person::renderTarget(glm::mat4 &view, glm::mat4 &projection) {
    leftArm.render(view,projection);
    rightArm.render(view,projection);
    leftLeg.render(view,projection);
    rightLeg.render(view,projection);
    head.render(view,projection);
    body.render(view,projection);
}
