//
// Created by kinghell on 19-5-29.
//

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <mutex>
#include <string>
#include <iostream>
#include "people/Target.h"
#include "people/Person.h"


using namespace std;

class Animation {
public:
    Animation(vector<Person *> *people, GLFWwindow **window);

    void start(string name);

private:
    static void running(string name, vector<Person *> *people);

    vector<Person *> *people;
    GLFWwindow **window;
};


