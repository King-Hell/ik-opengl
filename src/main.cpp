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
#include "Ground.h"

#define STB_IMAGE_IMPLEMENTATION

#include "Skybox.h"
#include "DepthMap.h"
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//窗口大小
GLuint SCR_WIDTH = 1024, SCR_HEIGHT = 1024;

//函数声明
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void Do_Movement(Target *target, MultiChain *multiChain = NULL);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool keys[1024];
GLfloat lastX = (GLfloat) SCR_WIDTH / 2, lastY = (GLfloat) SCR_HEIGHT / 2;
bool firstMouse = true;
bool headFlag = false;
bool moveFlag = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Target *tars[6];
Target *tar;

//主函数
int main() {
    //GLFW初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                          "Computer graphics experiment based on IK, SDU CS Li Tong",
                                          nullptr, nullptr);
    glfwMakeContextCurrent(window);

    //设置键盘鼠标回调函数
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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
    //加载模型
    Target target(0, 0, 0);
    Target leftArm(-0.8f, 0, 0);
    Target rightArm(0.8f, 0, 0);
    Target leftLeg(-0.6f, -1.5f, 0);
    Target rightLeg(0.6f, -1.5f, 0);
    Target head(0, 0.5f, 0);
    Target body(0, -0.5f, 0);
    tars[0] = &head;
    tars[1] = &leftArm;
    tars[2] = &rightArm;
    tars[3] = &leftLeg;
    tars[4] = &rightLeg;
    tars[5] = &body;
    tar = tars[1];
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
    MultiChain multichain(vec);
    Skybox skybox;
    Ground ground;
    DepthMap depthMap;
    //设置光线
    glm::vec3 lightPos(-1.0f, 1.0f, 1.0f);
    GLfloat near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0), glm::vec3(0, 1.0f, 0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    while (!glfwWindowShouldClose(window)) {
        /*渲染主循环*/
        //获取帧间隔
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //检查回调函数
        glfwPollEvents();
        //清空缓冲区
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //计算透视矩阵和视图矩阵
        glm::mat4 projection = glm::perspective(camera.Zoom, (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        Do_Movement(tar, &multichain);
        multichain.solve();
        depthMap.begin();
        multichain.renderDepthMap(lightSpaceMatrix);
        ground.renderDepthMap(lightProjection);
        depthMap.end(SCR_WIDTH, SCR_HEIGHT);
        multichain.render(view, projection, lightSpaceMatrix, depthMap.getDepthMap());
        ground.render(view, projection, lightSpaceMatrix, depthMap.getDepthMap());
        for (int i = 0; i < 6; i++) {
            tars[i]->render(view, projection);
        }
        view = glm::mat4(glm::mat3(camera.getViewMatrix()));
        skybox.render(view, projection);
        //交换缓冲
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void Do_Movement(Target *target, MultiChain *multiChain) {
    //执行动作
    if (headFlag) {
        if ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_UP])
            multiChain->moveHead(FORWARD, deltaTime);
        else if (keys[GLFW_KEY_UP])
            multiChain->moveHead(UP, deltaTime);
        if ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_DOWN])
            multiChain->moveHead(BACKWARD, deltaTime);
        else if (keys[GLFW_KEY_DOWN])
            multiChain->moveHead(DOWN, deltaTime);

        if (keys[GLFW_KEY_LEFT])
            multiChain->moveHead(LEFT, deltaTime);
        if (keys[GLFW_KEY_RIGHT])
            multiChain->moveHead(RIGHT, deltaTime);
    } else {
        if ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_UP])
            target->processTranslation(FORWARD, deltaTime);
        else if (keys[GLFW_KEY_UP])
            target->processTranslation(UP, deltaTime);

        if ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_DOWN])
            target->processTranslation(BACKWARD, deltaTime);
        else if (keys[GLFW_KEY_DOWN])
            target->processTranslation(DOWN, deltaTime);

        if (keys[GLFW_KEY_LEFT])
            target->processTranslation(LEFT, deltaTime);
        if (keys[GLFW_KEY_RIGHT])
            target->processTranslation(RIGHT, deltaTime);
    }
}


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    //键盘回调
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
    if (action == GLFW_PRESS && key >= GLFW_KEY_1 && key <= GLFW_KEY_6)
        tar = tars[key - GLFW_KEY_1];
    if (action == GLFW_PRESS && key == GLFW_KEY_Q)
        headFlag = !headFlag;
    if (action == GLFW_PRESS && key == GLFW_KEY_C) {
        if (moveFlag)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        moveFlag = !moveFlag;
    }

}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    //鼠标回调
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        fprintf(stderr, "Click\n");
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {//鼠标事件
    if (firstMouse) {
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }
    float xoffset = (float) xpos - lastX;
    float yoffset = lastY - (float) ypos; // reversed since y-coordinates go from bottom to top
    lastX = (float) xpos;
    lastY = (float) ypos;
    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {//滚轮事件
    camera.processMouseScroll(yoffset);
}