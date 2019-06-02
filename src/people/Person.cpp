//
// Created by kinghell on 19-5-31.
//

#include "Person.h"

Person::Person(glm::vec3 position) {
    //加载模型
    leftArm = Target(glm::vec3(-0.8f, 0, 0) + position);
    rightArm = Target(glm::vec3(0.8f, 0, 0) + position);
    leftLeg = Target(glm::vec3(-0.6f, -1.5f, 0) + position);
    rightLeg = Target(glm::vec3(0.6f, -1.5f, 0) + position);
    head = Target(glm::vec3(0, 0.5f, 0) + position);
    body = Target(glm::vec3(0, -0.5f, 0) + position);
    vector<Chain *> vec;
    vector<glm::vec3> headJoints;
    headJoints.emplace_back(glm::vec3(0, 0.5f, 0) + position);
    headJoints.emplace_back(glm::vec3(0, 0.1f, 0) + position);
    headJoints.emplace_back(glm::vec3(0, 0, 0) + position);
    vector<glm::vec2> headScales;
    headScales.emplace_back(0.4f, 0.4f);
    headScales.emplace_back(0.1f, 0.1f);
    Chain *headNeck = new Chain(headJoints, &head, headScales);
    headNeck->setTexture(0, "res/textures/ironman.png");
    vec.push_back(headNeck);
    vec.push_back(new Chain(glm::vec3(0, 0, 0) + position, glm::vec3(-0.8f, 0, 0) + position, &leftArm, 2));
    vec.push_back(new Chain(glm::vec3(0, 0, 0) + position, glm::vec3(0.8f, 0, 0) + position, &rightArm, 2));
    vec.push_back(new Chain(glm::vec3(0, 0, 0) + position, glm::vec3(0, -0.7f, 0) + position, &body, 1, 0.4f, 0.15f));
    vec.push_back(
            new Chain(glm::vec3(0, -0.7f, 0) + position, glm::vec3(-0.6f, -1.5f, 0) + position, &leftLeg, 2, 0.12f,
                      0.12f));
    vec.push_back(
            new Chain(glm::vec3(0, -0.7f, 0) + position, glm::vec3(0.6f, -1.5f, 0) + position, &rightLeg, 2, 0.12f,
                      0.12f));
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

void Person::select(int i) {
    switch (i) {
        case 1:
            selected = &leftArm;
            break;
        case 2:
            selected = &rightArm;
            break;
        case 3:
            selected = &leftLeg;
            break;
        case 4:
            selected = &rightLeg;
            break;
        default:
            cout << "Error Key!" << endl;
    }
}
