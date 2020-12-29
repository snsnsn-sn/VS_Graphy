#pragma once
#include<GL/glut.h>
#include"model.h"
#include"Glm/glm.hpp"
#include"Glm/ext.hpp"

//   定义全局变量

// matrix
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
glm::mat4 normalMatrix;
glm::mat4 lightProjectionMatrix;
glm::mat4 lightViewMatrix;

//  texture and program
GLuint roomTexture, shadowMap;
GLuint renderProgram, depthProgram;
GLuint flowerpotTexture, parterreTexture;

// 渲染所用参数
GLuint renderPosLoc, renderTexcoordLoc, renderNormalLoc;
GLuint renderMLoc, renderVLoc, renderPLoc, renderNMLoc, renderViewPosLocation;
GLuint renderLightVMatrixLoc, renderLightPMatrixLoc;
GLuint renderLightOnLoc;
GLuint renderTextureLoc, shadowMapLoc;
GLuint depthPosLoc, depthTexcoordLoc, depthNormalLoc;
GLuint depthMLoc, depthVLoc, depthPLoc;

GLint changecolor = 0;//改变纹理颜色

// VertexData  and  VBO、IBO
VertexData* roomVertexData;
int roomVertexCount, roomIndexCount;
unsigned int* roomIndexes;
GLuint roomVbo, roomIbo;

VertexData* roomVertexData2;
int roomVertexCount2, roomIndexCount2;
unsigned int* roomIndexes2;
GLuint roomVbo2, roomIbo2;

// camera
glm::vec3 cameraPos;//摄像机位置
glm::vec3 cameraCenter;
glm::vec3 cameraForward;

glm::vec3 cameraTarget;//原点位置
glm::vec3 cameraDirection;//摄像机朝向
glm::vec3 up; //上向量
glm::vec3 cameraRight;//右向量
glm::vec3 cameraUp;//上向量
float cameraSpeed = 2.5f;//方向移动速度