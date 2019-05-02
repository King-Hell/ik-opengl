//STD
#include <string>
#include <iostream>
#include <cstring>
//GLAD
#include <glad/glad.h>
//GLFW
#include <GLFW/glfw3.h>
//Project
#include "Shader.h"
#include "Camera.h"
#include "Target.h"
#include "Chain.h"
#include "MultiChain.h"
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//窗口大小
GLuint screenWidth = 1600, screenHeight = 900;

//函数声明
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void Do_Movement(Target *target);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Target* tars[6];
Target* tar;
//主函数
int main() {

    char desired_model[1000];
    cout
            << "IK OpenGL - Jesse & Robb - CSE 163, Project 4, SP17\n1: Single chain\n2: Multichain\n3: Single Chain w/ Constraint\nEnter the model you want here: ";
    //cin >> desired_model;
    desired_model[0]='2';
    desired_model[1]='\0';
    cout << desired_model << endl;
    //GLFW初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight,
                                          "Fast and Backwards Reaching Inverse Kinematic (FABRIK) Solver - Robb & Jesse - CSE 163",
                                          nullptr, nullptr);
    glfwMakeContextCurrent(window);

    //设置键盘鼠标回调函数
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    //初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //设置绘图区域位置大小
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    //开启深度测试和多重抗锯齿
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Load joints
    vector<glm::vec3> joints1;
    for (int i = 0; i < 10; ++i) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        joints1.push_back(glm::vec3(0, r, 0));
    }

    vector<glm::vec3> joints2;
    joints2.push_back(glm::vec3(0, 0.0f, 0));
    joints2.push_back(glm::vec3(0, 1.0f, 0));
    joints2.push_back(glm::vec3(1.0f, 2.0f, 0));

    //加载模型
    //Target target(0, 0, 0);
    Target target2(-0.8f, 0, 0);
    Target target3(0.8f, 0, 0);
    Target target4(-0.8f, -1.0f, 0);
    Target target5(0.8f, -1.0f, 0);
    Target target6(0, 0.5f, 0);
    //Chain chain1(joints1, &target);
    //Chain chain2(joints2, &target);
    //chain2.please_constrain = true;
    tars[1]=&target2;
    tars[2]=&target3;
    tars[3]=&target4;
    tars[4]=&target5;
    tars[5]=&target6;
    tar=&target2;
    vector<Chain *> vec;
/*    vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), &target));
    vec.push_back(new Chain(glm::vec3(0, 1, 0), glm::vec3(1, 1, 0), &target, 2));
    vec.push_back(new Chain(glm::vec3(0, 1, 0), glm::vec3(-1, 2, 0), &target3, 2));
    vec.push_back(new Chain(glm::vec3(0, 1, 0), glm::vec3(-1, 1.5, 0), &target2, 2));*/
    vec.push_back(new Chain(glm::vec3(0, 0.5f, 0), glm::vec3(0, 0, 0), &target6,2));
    vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(-1.0f, 0, 0), &target2, 2));
    vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(1.0f, 0, 0), &target3, 2));
    vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(-1.0f, -1.0f, 0), &target4, 2));
    vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(1.0f, -1.0f, 0), &target5, 2));
    MultiChain multichain(vec);

    // Game loop
    while (!glfwWindowShouldClose(window)) {

        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();

        // Clear the colorbuffer
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Do_Movement(tar);

        // Transformation matrices
        glm::mat4 projection = glm::perspective(camera.Zoom, (float) screenWidth / (float) screenHeight, 0.1f, 100.0f);

        glm::mat4 view = camera.GetViewMatrix();
        //target.Render(view, projection);

        if (strcmp(desired_model, "1") == 0) {
            //chain1.Solve();
            //chain1.Render(view, projection);
        } else if (strcmp(desired_model, "2") == 0) {
            multichain.Solve();
            multichain.Render(view, projection);
            target2.Render(view, projection);
            target3.Render(view, projection);
            target4.Render(view, projection);
            target5.Render(view, projection);
            target6.Render(view, projection);
        } else if (strcmp(desired_model, "3") == 0) {
            //chain2.Solve();
            //chain2.Render(view, projection);
        } else {
            cout << "Invalid chain model" << endl;
            break;
        }

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

#pragma region "User input"

// Moves/alters the target position based on user input
void Do_Movement(Target *target) {

    if ((keys[GLFW_KEY_LEFT_SHIFT]||keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_UP])
        target->ProcessTranslation(FORWARD, deltaTime);
    else if (keys[GLFW_KEY_UP])
        target->ProcessTranslation(UP, deltaTime);

    if ((keys[GLFW_KEY_LEFT_SHIFT]||keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_DOWN])
        target->ProcessTranslation(BACKWARD, deltaTime);
    else if (keys[GLFW_KEY_DOWN])
        target->ProcessTranslation(DOWN, deltaTime);

    if (keys[GLFW_KEY_LEFT])
        target->ProcessTranslation(LEFT, deltaTime);
    if (keys[GLFW_KEY_RIGHT])
        target->ProcessTranslation(RIGHT, deltaTime);
}


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    //键盘回调
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
    if(action==GLFW_PRESS&&key>=GLFW_KEY_2&&key<=GLFW_KEY_6)
        tar=tars[key-GLFW_KEY_2+1];


}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    //鼠标回调
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        fprintf(stderr, "Click\n");
    }
}

#pragma endregion

// Leap motion stuff

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

