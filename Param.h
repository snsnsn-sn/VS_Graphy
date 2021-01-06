#pragma once
#include<GL/freeglut.h>
#include"model.h"
#include"Glm/glm.hpp"
#include"Glm/ext.hpp"

//   ����ȫ�ֱ���

// matrix
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
glm::mat4 normalMatrix;
glm::mat4 lightProjectionMatrix;
glm::mat4 lightViewMatrix;

//ǽ�ڡ��ذ塢�ڻ��������л�
std::vector<GLuint> paintings;//�����һ�����
std::vector<GLuint> walls;//����ǽ������
std::vector<GLuint> floors;//�����ذ�����
std::vector<GLuint> otherPaintings;//�������߹һ�����
int floorsSize;//vector�ĳ���
int wallsSize;
int paintingsSize;
int OtherPaintingsSize;
GLuint floorTexture = 0;//vector�ĵڼ�������
GLuint wallTexture = 0;
GLuint paintTexture = 0;
GLuint otherPaintTexture = 0;

//  texture and program
GLuint roomTexture, shadowMap;
GLuint renderProgram, depthProgram;
GLuint flowerpotTexture, parterreTexture;

// ��Ⱦ���ò���
GLuint renderPosLoc, renderTexcoordLoc, renderNormalLoc;
GLuint renderMLoc, renderVLoc, renderPLoc, renderNMLoc, renderViewPosLocation;
GLuint renderLightVMatrixLoc, renderLightPMatrixLoc;
GLuint renderLightOnLoc;
GLuint renderTextureLoc, shadowMapLoc;
GLuint depthPosLoc, depthTexcoordLoc, depthNormalLoc;
GLuint depthMLoc, depthVLoc, depthPLoc;



// VertexData  and  VBO��IBO
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
//����=================================
//ǽ�� 
VertexData* wallVertexData;
VertexData* wall_yellow;
VertexData* wall_green;
VertexData* wall_white;
VertexData* wall_orange;
VertexData* wall_blue;

int wallVertexCount, wallIndexCount;
unsigned int* wallIndexes;
GLuint wallVbo, wallIbo;

GLuint wallYellowVbo, wallGreenVbo, wallWhiteVbo, wallOrangeVbo, wallBlueVbo;//��ͬ��ɫǽ�ڵ�vbo

int changeWall = 0;//�л�����ǽ�ڵ���ɫ
int changeBed = 0;//�л���
int changeCurtain = 0;//�л�����

int changeStyle = 0;//�л����䣬�ɱ�ɵ���ģ�ͺͲ��ģ��
//�ڲ�
VertexData* roomVertexData;
int roomVertexCount, roomIndexCount;
unsigned int* roomIndexes;
GLuint roomVbo, roomIbo;
//����================================
//ǽ��
VertexData* wallVertexData2;
int wallVertexCount2, wallIndexCount2;
unsigned int* wallIndexes2;
GLuint wallVbo2, wallIbo2;

//��1
VertexData* bedVertexData;
int bedVertexCount, bedIndexCount;
unsigned int* bedIndexes;
GLuint bedVbo, bedIbo;

//��2
VertexData* bedVertexData2;
int bedVertexCount2, bedIndexCount2;
unsigned int* bedIndexes2;
GLuint bedVbo2, bedIbo2;

//����
VertexData* curtainVertexData;
int curtainVertexCount, curtainIndexCount;
unsigned int* curtainIndexes;
GLuint curtainVbo, curtainIbo;

//����
VertexData* windowVertexData;
int windowVertexCount, windowIndexCount;
unsigned int* windowIndexes;
GLuint windowVbo, windowIbo;

//�ذ�
VertexData* floorVertexData;
int floorVertexCount, floorIndexCount;
unsigned int* floorIndexes;
GLuint floorVbo, floorIbo;

//װ��1
VertexData* decoVertexData;
int decoVertexCount, decoIndexCount;
unsigned int* decoIndexes;
GLuint decoVbo, decoIbo;

//װ��2
VertexData* decoVertexData2;
int decoVertexCount2, decoIndexCount2;
unsigned int* decoIndexes2;
GLuint decoVbo2, decoIbo2;


// camera
glm::vec3 cameraPos;//�����λ��
glm::vec3 cameraCenter;
glm::vec3 cameraForward;

glm::vec3 cameraTarget;//ԭ��λ��
glm::vec3 cameraDirection;//���������
glm::vec3 up; //������
glm::vec3 cameraRight;//������
glm::vec3 cameraUp;//������
float cameraSpeed = 2.5f;//�����ƶ��ٶ�

float fov = 60.0f; //������ӽǣ�ͨ�����������֣��Ӷ��������Ӽ�С�ӽǣ�