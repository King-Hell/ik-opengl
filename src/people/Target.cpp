#include "Target.h"
#include "../utils/Camera.h"

Target::Target(glm::vec3 pos) {
    /*构造函数*/
    //加载着色器
    Shader modelS(vertexShaderPath, fragShaderPath);
    objectShader = modelS;
    //加载模型
    Model modelM(pathToModel);
    objectModel = modelM;
    //目标点需要增加部分偏移量防止算法出错
    position = pos + 0.0001f;
    scale = glm::vec3(.05f, .05f, .05f);
    pitch = 0.0f;
    yaw = 0.0f;
}

void Target::render(glm::mat4 &view, glm::mat4 &projection) {
    /*渲染函数*/
    objectShader.use();
    //设置片段着色器 物体颜色 光照颜色 光照位置 视角位置
    objectShader.setFloat3("objectColor", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat3("lightColor", 1.0f, 0.0f, 0.0f);
    objectShader.setFloat3("lightPos", 1.0f, 1.0f, 3.0f);
    objectShader.setFloat3("viewPos", 0.0, 0.0, 3.0);
    //设置顶点着色器 位移 缩放 旋转
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
    R = glm::rotate(R, yaw, glm::vec3(0, 0, 1));
    glm::mat4 model = T * R * S;
    objectShader.setMat4("model",model);
    objectShader.setMat4("view",view);
    objectShader.setMat4("projection",projection);
    //模型渲染
    objectModel.draw(objectShader);
}

void Target::processTranslation(Camera_Movement direction, GLfloat deltaTime) {
    /*位移处理方法*/
    GLfloat velocity = 2.0f * deltaTime;
    if (direction == UP)
        this->position.y += 1.0f * velocity;
    if (direction == DOWN){
        if (this->position.y>-1.5f)
            this->position.y -= 1.0f * velocity;
    }
    if (direction == LEFT)
        this->position.x -= 1.0f * velocity;
    if (direction == RIGHT)
        this->position.x += 1.0f * velocity;
    if (direction == FORWARD)
        this->position.z += 1.0f * velocity;
    if (direction == BACKWARD)
        this->position.z -= 1.0f * velocity;
}
