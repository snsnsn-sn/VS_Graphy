#pragma once
#include<GL/freeglut.h>
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

//墙壁、地板、壁画等纹理切换
std::vector<GLuint> paintings;//客厅挂画纹理
std::vector<GLuint> walls;//客厅墙壁纹理
std::vector<GLuint> floors;//客厅地板纹理
std::vector<GLuint> otherPaintings;//客厅两边挂画纹理
int floorsSize;//vector的长度
int wallsSize;
int paintingsSize;
int OtherPaintingsSize;
GLuint floorTexture = 0;//vector的第几个纹理
GLuint wallTexture = 0;
GLuint paintTexture = 0;
GLuint otherPaintTexture = 0;

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



// VertexData  and  VBO、IBO
//VertexData* roomVertexData;
//int roomVertexCount, roomIndexCount;
//unsigned int* roomIndexes;
//GLuint roomVbo, roomIbo;
//
VertexData* roomVertexData2;
int roomVertexCount2, roomIndexCount2;
unsigned int* roomIndexes2;
GLuint roomVbo2, roomIbo2;
//
//VertexData* roomVertexData3;
//int roomVertexCount3, roomIndexCount3;
//unsigned int* roomIndexes3;
//GLuint roomVbo3, roomIbo3;
//客厅=================================
//墙壁 
VertexData* wallVertexData;
VertexData* wall_yellow;
VertexData* wall_green;
VertexData* wall_white;
VertexData* wall_orange;
VertexData* wall_blue;

int wallVertexCount, wallIndexCount;
unsigned int* wallIndexes;
GLuint wallVbo, wallIbo;

GLuint wallYellowVbo, wallGreenVbo, wallWhiteVbo, wallOrangeVbo, wallBlueVbo;//不同颜色墙壁的vbo

int changeWall = 0;//切换客厅墙壁的颜色
int changeBed = 0;//切换床
int changeCurtain = 0;//切换窗帘

int changeStyle = 0;//切换房间，可变成单独模型和拆分模型
//内部
VertexData* roomVertexData;
int roomVertexCount, roomIndexCount;
unsigned int* roomIndexes;
GLuint roomVbo, roomIbo;
//卧室================================
//墙壁
VertexData* wallVertexData2;
int wallVertexCount2, wallIndexCount2;
unsigned int* wallIndexes2;
GLuint wallVbo2, wallIbo2;

//床1
VertexData* bedVertexData;
int bedVertexCount, bedIndexCount;
unsigned int* bedIndexes;
GLuint bedVbo, bedIbo;

//床2
VertexData* bedVertexData2;
int bedVertexCount2, bedIndexCount2;
unsigned int* bedIndexes2;
GLuint bedVbo2, bedIbo2;

//窗帘
VertexData* curtainVertexData;
int curtainVertexCount, curtainIndexCount;
unsigned int* curtainIndexes;
GLuint curtainVbo, curtainIbo;

//窗户
VertexData* windowVertexData;
int windowVertexCount, windowIndexCount;
unsigned int* windowIndexes;
GLuint windowVbo, windowIbo;

//地板
VertexData* floorVertexData;
int floorVertexCount, floorIndexCount;
unsigned int* floorIndexes;
GLuint floorVbo, floorIbo;

//装饰1
VertexData* decoVertexData;
int decoVertexCount, decoIndexCount;
unsigned int* decoIndexes;
GLuint decoVbo, decoIbo;

//装饰2
VertexData* decoVertexData2;
int decoVertexCount2, decoIndexCount2;
unsigned int* decoIndexes2;
GLuint decoVbo2, decoIbo2;


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

float fov = 60.0f; //摄像机视角，通过监听鼠标滚轮，从而触发增加减小视角，