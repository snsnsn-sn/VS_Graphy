#pragma once
#define NDEBUG 
#include<GL/freeglut.h>
#include"model.h"
#include"Glm/glm.hpp" //OpenGL Mathematics ����OpenGL��ɫ���ԣ�GLSL���淶��ͼ�������ͷ�ļ�C ++��ѧ�⡣
#include"Glm/ext.hpp"
#include<vector>

//   ����ȫ�ֱ���

// matrix
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
glm::mat4 normalMatrix;
glm::mat4 lightProjectionMatrix;
glm::mat4 lightViewMatrix;

//  texture and program
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


GLuint livingRoomWallTexture; //����ǽ���������
GLuint roomTexture, shadowMap;  //�����������Ӱ
GLuint renderProgram, depthProgram, livingRoomWallProgram;
GLuint flowerpotTexture, parterreTexture;

// ��Ⱦ���ò���
GLuint renderPosLoc, renderTexcoordLoc, renderNormalLoc;
GLuint renderMLoc, renderVLoc, renderPLoc, renderNMLoc, renderViewPosLocation;
GLuint renderLightVMatrixLoc, renderLightPMatrixLoc;
GLuint renderLightOnLoc;
GLuint renderTextureLoc, shadowMapLoc;

GLfloat rendersunlight;

GLuint depthPosLoc, depthTexcoordLoc, depthNormalLoc;
GLuint depthMLoc, depthVLoc, depthPLoc;


//----------------------------------------------------
//����ģ����ر���
//----------------------------------------------------
//����
VertexData* tableVertexData;
int tableVertexCount, tableIndexCount;
unsigned int* tableIndexes;
GLuint tableVbo, tableIbo;

//����
VertexData* drawerVertexData;
int drawerVertexCount, drawerIndexCount;
unsigned int* drawerIndexes;
GLuint drawerVbo, drawerIbo;

//����2
VertexData* drawerVertexData2;
int drawerVertexCount2, drawerIndexCount2;
unsigned int* drawerIndexes2;
GLuint drawerVbo2, drawerIbo2;

//ǽ��
VertexData* wallVertexData;
int wallVertexCount, wallIndexCount;
unsigned int* wallIndexes;
GLuint wallVbo, wallIbo;

//�ذ�
VertexData* floorVertexData;
int floorVertexCount, floorIndexCount;
unsigned int* floorIndexes;
GLuint floorVbo, floorIbo;

//������Ʒ
VertexData* kitchenVertexData;
int kitchenVertexCount, kitchenIndexCount;
unsigned int* kitchenIndexes;
GLuint kitchenVbo, kitchenIbo;

//�鷿��Ʒ
VertexData* studyRoomVertexData;
int studyRoomVertexCount, studyRoomIndexCount;
unsigned int* studyRoomIndexes;
GLuint studyRoomVbo, studyRoomIbo;

//����
VertexData* roomVertexData;
int roomVertexCount, roomIndexCount;
unsigned int* roomIndexes;
GLuint roomVbo, roomIbo;

//����  ǽ��--��1
VertexData* pVertexData;
int pVertexCount, pIndexCount;
unsigned int* pIndexes;
GLuint pVbo, pIbo;

//����  ǽ��--��2
VertexData* p2VertexData;
int p2VertexCount, p2IndexCount;
unsigned int* p2Indexes;
GLuint p2Vbo, p2Ibo;

//���� ��--˫�˴�
VertexData* bedVertexData;
int bedVertexCount, bedIndexCount;
unsigned int* bedIndexes;
GLuint bedVbo, bedIbo;

//���� ��--���˴�
VertexData* bed2VertexData;
int bed2VertexCount, bed2IndexCount;
unsigned int* bed2Indexes;
GLuint bed2Vbo, bed2Ibo;

//���� ��ǽ--������ʽ1
VertexData* window1VertexData;
int window1VertexCount, window1IndexCount;
unsigned int* window1Indexes;
GLuint window1Vbo, window1Ibo;

//���� ��ǽ--������ʽ2
VertexData* window2VertexData;
int window2VertexCount, window2IndexCount;
unsigned int* window2Indexes;
GLuint window2Vbo, window2Ibo;

//���� ��ǽ--������ʽ3
VertexData* window3VertexData;
int window3VertexCount, window3IndexCount;
unsigned int* window3Indexes;
GLuint window3Vbo, window3Ibo;

//���� ��ǽ--������ʽ4
VertexData* window4VertexData;
int window4VertexCount, window4IndexCount;
unsigned int* window4Indexes;
GLuint window4Vbo, window4Ibo;

//���� ��ǽ--װ��1 �յ�����
VertexData* decoVertexData;
int decoVertexCount, decoIndexCount;
unsigned int* decoIndexes;
GLuint decoVbo, decoIbo;

//���� ��ǽ--װ��2 �鼮
VertexData* deco2VertexData;
int deco2VertexCount, deco2IndexCount;
unsigned int* deco2Indexes;
GLuint deco2Vbo, deco2Ibo;

//���� ������Ʒ
VertexData* bedroomVertexData;
int bedroomVertexCount, bedroomIndexCount;
unsigned int* bedroomIndexes;
GLuint bedroomVbo, bedroomIbo;

//----------------------------------------
//ģ����ر����������
//----------------------------------------

//----------------------------------------
//�����������ر���
//----------------------------------------
glm::vec3 cameraPos;//�����λ��
glm::vec3 cameraCenter;
glm::vec3 cameraForward;

glm::vec3 cameraTarget;//ԭ��λ��
glm::vec3 cameraDirection;//���������
glm::vec3 up; //������
glm::vec3 cameraRight;//������
glm::vec3 cameraUp;//������
float cameraSpeed = 1.25f; //�ƶ��ٶ�
float fov = 60.0f; //������ӽǣ�ͨ�����������֣��Ӷ��������Ӽ�С�ӽǣ������Ŵ���С��Ч����
//----------------------------------------
//�������ر����������
//----------------------------------------