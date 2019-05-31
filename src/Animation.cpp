//
// Created by kinghell on 19-5-29.
//
#include "Animation.h"
#define GLFW_KEY_LEFT_SHIFT         340
#define GLFW_KEY_UP                 265
Animation::Animation(Setting *setting) {
    this->setting=setting;
}

void Animation::running(Setting *setting) {
/*    setting->tar=setting->tars[3];
    setting->keys[GLFW_KEY_LEFT_SHIFT]=true;
    setting->keys[GLFW_KEY_UP]=true;
    this_thread::sleep_for(chrono::milliseconds(500));
    setting->keys[GLFW_KEY_UP]=false;
    setting->tar=setting->tars[4];
    setting->keys[GLFW_KEY_UP]=true;
    this_thread::sleep_for(chrono::milliseconds(500));
    setting->keys[GLFW_KEY_UP]=false;
    setting->keys[GLFW_KEY_LEFT_SHIFT]=false;*/
    setting->tars[3]->position+=glm::vec3(0.3f,0,0);
    setting->tars[4]->position-=glm::vec3(0.3f,0,0);
    setting->tars[3]->position-=glm::vec3(0,0,1);
    setting->tars[4]->position+=glm::vec3(0,0,1);
    this_thread::sleep_for(chrono::milliseconds(500));
    setting->tars[3]->position+=glm::vec3(0,0,1);
    setting->tars[4]->position-=glm::vec3(0,0,1);
    this_thread::sleep_for(chrono::milliseconds(500));
    setting->tars[3]->position+=glm::vec3(0,0,1);
    setting->tars[4]->position-=glm::vec3(0,0,1);
    this_thread::sleep_for(chrono::milliseconds(500));
    setting->tars[3]->position-=glm::vec3(0,0,1);
    setting->tars[4]->position+=glm::vec3(0,0,1);
}

void Animation::start(string name) {
    if (name == "running"){
        thread t(running,setting);
        t.detach();
    }

}


