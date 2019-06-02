//
// Created by kinghell on 19-5-29.
//
#include "Animation.h"

#define GLFW_KEY_LEFT_SHIFT         340
#define GLFW_KEY_UP                 265
mutex amLock;
bool keys[1024];

Animation::Animation(vector<Person *> *people, GLFWwindow **window) {
    this->people = people;
    this->window = window;
}

void Animation::running(string name, vector<Person *> *people) {
    ifstream file;
    file.open("src/action/" + name);
    if (file.fail()) {
        cout << "File to open src/action/" << name << endl;
        return;
    }
    uint key;
    float time;
    this_thread::sleep_for(chrono::seconds(1));
    while (!file.eof()) {
        file >> key;
        file >> time;
        if (key >= GLFW_KEY_2 && key <= GLFW_KEY_5) {
            for (uint i = 0; i < people->size(); i++)
                people->at(i)->select(key - GLFW_KEY_1);
        } else {
            keys[key] = true;
            this_thread::sleep_for(chrono::milliseconds((int) (time * 1000)));
            keys[key] = false;
        }
    }
    file.close();
}

void Animation::start(string name) {
    for (int i = 1; i <= 9; i++) {
        people->emplace_back(new Person(glm::vec3(-i, 0, -i)));
    }
    thread t(running, name, people);
    t.detach();


}


