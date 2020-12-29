#pragma once
#include<GL/glut.h>
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

GLint changecolor = 0;//�ı�������ɫ

// VertexData  and  VBO��IBO
VertexData* roomVertexData;
int roomVertexCount, roomIndexCount;
unsigned int* roomIndexes;
GLuint roomVbo, roomIbo;

VertexData* roomVertexData2;
int roomVertexCount2, roomIndexCount2;
unsigned int* roomIndexes2;
GLuint roomVbo2, roomIbo2;

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