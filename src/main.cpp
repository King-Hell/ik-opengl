//STD
#include <string>
#include <iostream>
#include <cstring>
//GLAD
#include <glad/glad.h>
//GLFW
#include <GLFW/glfw3.h>
//Project
#include "utils/Shader.h"
#include "utils/Camera.h"
#include "people/Target.h"
#include "people/Chain.h"
#include "people/MultiChain.h"
#include "background/Ground.h"
#include "Animation.h"
#include "people/Person.h"
#include "utils/DepthMap.h"

#define STB_IMAGE_IMPLEMENTATION

#include "background/Skybox.h"

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//窗口大小
GLuint SCR_WIDTH = 1024, SCR_HEIGHT = 1024;

//函数声明
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void Do_Movement(Person &person);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
GLfloat lastX = (GLfloat) SCR_WIDTH / 2, lastY = (GLfloat) SCR_HEIGHT / 2;
bool firstMouse = true;
bool headFlag = false;
bool moveFlag = false;
extern bool keys[];
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLFWwindow *window = NULL;
extern mutex amLock;
vector<Person *> people;
Animation animation(&people, &window);

GLfloat tstart, tend;

//主函数
int main() {
    //TODO:增加动画功能
    //GLFW初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
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
    people.emplace_back(new Person(glm::vec3(0, 0, 0)));
/*    people.emplace_back(new Person(glm::vec3(-2.0f,0,0)));
    people.emplace_back(new Person(glm::vec3(2.0f,0,0)));
    people.emplace_back(new Person(glm::vec3(-1.0f,2.0f,0)));
    people.emplace_back(new Person(glm::vec3(1.0f,2.0f,0)));*/
    Skybox skybox;
    Ground ground;
    DepthMap depthMap;
    //设置光线
    glm::vec3 lightPos(-1.0f, 1.0f, 1.0f);
    GLfloat near_plane = -7.5f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0), glm::vec3(0, 1.0f, 0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    while (!glfwWindowShouldClose(window)) {
        /*渲染主循环*/
        //获取帧间隔
        amLock.lock();
        glfwMakeContextCurrent(window);
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
        depthMap.begin();
        for (uint i = 0; i < people.size(); i++) {
            Do_Movement(*people[i]);
            people[i]->solve();
            people[i]->renderDepthMap(lightSpaceMatrix);
        }
        ground.renderDepthMap(lightProjection);
        depthMap.end(SCR_WIDTH, SCR_HEIGHT);
        for (uint i = 0; i < people.size(); i++) {
            people[i]->render(view, projection, lightSpaceMatrix, depthMap.getDepthMap());
            //people[i].renderTarget(view, projection);
        }
        people[0]->renderTarget(view, projection);
        ground.render(view, projection, lightSpaceMatrix, depthMap.getDepthMap());
        view = glm::mat4(glm::mat3(camera.getViewMatrix()));
        skybox.render(view, projection);
        //交换缓冲
        glfwSwapBuffers(window);
        glfwMakeContextCurrent(NULL);
        amLock.unlock();
    }
    glfwTerminate();
    return 0;
}

void Do_Movement(Person &person) {
    //执行动作
    if (headFlag) {
        if ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_UP])
            person.moveHead(FORWARD, deltaTime);
        else if (keys[GLFW_KEY_UP])
            person.moveHead(UP, deltaTime);
        if ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_DOWN])
            person.moveHead(BACKWARD, deltaTime);
        else if (keys[GLFW_KEY_DOWN])
            person.moveHead(DOWN, deltaTime);

        if (keys[GLFW_KEY_LEFT])
            person.moveHead(LEFT, deltaTime);
        if (keys[GLFW_KEY_RIGHT])
            person.moveHead(RIGHT, deltaTime);
    } else {
        if ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_UP])
            person.processTranslation(FORWARD, deltaTime);
        else if (keys[GLFW_KEY_UP])
            person.processTranslation(UP, deltaTime);

        if ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) && keys[GLFW_KEY_DOWN])
            person.processTranslation(BACKWARD, deltaTime);
        else if (keys[GLFW_KEY_DOWN])
            person.processTranslation(DOWN, deltaTime);

        if (keys[GLFW_KEY_LEFT])
            person.processTranslation(LEFT, deltaTime);
        if (keys[GLFW_KEY_RIGHT])
            person.processTranslation(RIGHT, deltaTime);
    }
    if (keys[GLFW_KEY_W])
        camera.processTranslation(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.processTranslation(RIGHT, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.processTranslation(FORWARD, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.processTranslation(LEFT, deltaTime);
}


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    //键盘回调

    if (action == GLFW_PRESS) {
        keys[key] = true;
        tstart = glfwGetTime();
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_Q:
                headFlag = !headFlag;
                break;
            case GLFW_KEY_C:
                if (moveFlag)
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                else
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                moveFlag = !moveFlag;
                break;
            case GLFW_KEY_M:
                animation.start("dance");
                break;
            default:
                if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
                    for (uint i = 0; i < people.size(); i++) {
                        people[i]->select(key - GLFW_KEY_1);
                    }
                }
        }
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
        tend = glfwGetTime();
        cout << "Key:" << key << ",time:" << tend - tstart << endl;
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