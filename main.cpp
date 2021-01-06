#include"misc.h"
#include"FBO.h"
#include"Param.h"
#include<gl/freeglut.h>
#pragma comment(lib, "soil.lib")
#pragma comment(lib, "glew32.lib")

//���Ϲ���ģ��(Phong Lighting Model)->�ɻ���(Ambient)��������(Diffuse)�;���(Specular)���������������
FrameBufferObject* mFbo;

bool bCameraRotate = false;   //  ������Ƿ�������ת
float cameraXZRotateAngle = 0;   // �����xzƽ���ڵ���ת�Ƕ� ��������ת
float cameraYRotateAngle = 0;   //  �������ֱƽ���ڵ���ת�Ƕ�   ������ת

int lastMousePosX, lastMousePosY;  //  �ϴ�����λ��  

void DrawRoom();
void DrawRoomSample();

//------------------------------------
// ��Ʒ��������
//------------------------------------
int bed = 0;        //���Ҵ���ʽ
int wallDeco = 0;   //����ǽ��װ��
int window = 0;     //���Ҵ�����ʽ
int drawer = 0;     //������ʽ

int lightOn = 1;   // �Ƿ�������

float sunlight = 50.0;//��Դ��λ��

					  /**
* @����:��ȡģ��
* @˵��: VertexData: objģ�͵�ȫ����������
*        Vbo:���㻺�����,�洢����ĸ���������Ϣ
*        IBO:�����������,�洢��������,���ٴ��䶥������,���Ч��
*
*  ע:�����еı�����Param.h������
*/
void readModel() {
	//����ǽ��
	wallVertexData = LoadObjModel("Res/group1/wall.obj", &wallIndexes, wallVertexCount, wallIndexCount);
	wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wallVertexData);
	wallIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * wallIndexCount, GL_STATIC_DRAW, wallIndexes);

	//����ذ�
	floorVertexData = LoadObjModel("Res/group1/floor.obj", &floorIndexes, floorVertexCount, floorIndexCount);
	floorVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * floorVertexCount, GL_STATIC_DRAW, floorVertexData);
	floorIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * floorIndexCount, GL_STATIC_DRAW, floorIndexes);

	//�����ڲ�(����ǽ��)
	roomVertexData = LoadObjModel("Res/group1/livingRoom.obj", &roomIndexes, roomVertexCount, roomIndexCount);
	roomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount, GL_STATIC_DRAW, roomVertexData);
	roomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount, GL_STATIC_DRAW, roomIndexes);

	//����
	tableVertexData = LoadObjModel("Res/group1/table.obj", &tableIndexes, tableVertexCount, tableIndexCount);
	tableVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * tableVertexCount, GL_STATIC_DRAW, tableVertexData);
	tableIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tableIndexCount, GL_STATIC_DRAW, tableIndexes);

	//��������
	drawerVertexData = LoadObjModel("Res/group1/drawerIn.obj", &drawerIndexes, drawerVertexCount, drawerIndexCount);
	drawerVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * drawerVertexCount, GL_STATIC_DRAW, drawerVertexData);
	drawerIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * drawerIndexCount, GL_STATIC_DRAW, drawerIndexes);

	//��������2
	drawerVertexData2 = LoadObjModel("Res/group1/drawer.obj", &drawerIndexes2, drawerVertexCount2, drawerIndexCount2);
	drawerVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * drawerVertexCount2, GL_STATIC_DRAW, drawerVertexData2);
	drawerIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * drawerIndexCount2, GL_STATIC_DRAW, drawerIndexes2);

	//���� ǽ�����--�м䵥��
	pVertexData = LoadObjModel("Res/group1/painting1.obj", &pIndexes, pVertexCount, pIndexCount);
	pVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * pVertexCount, GL_STATIC_DRAW, pVertexData);
	pIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * pIndexCount, GL_STATIC_DRAW, pIndexes);

	//���� ǽ�����--�������
	p2VertexData = LoadObjModel("Res/group1/painting2.obj", &p2Indexes, p2VertexCount, p2IndexCount);
	p2Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * p2VertexCount, GL_STATIC_DRAW, p2VertexData);
	p2Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * p2IndexCount, GL_STATIC_DRAW, p2Indexes);

	//���� ��--1 ˫�˴�+ǰ����̺
	bedVertexData = LoadObjModel("Res/group2/bed1.obj", &bedIndexes, bedVertexCount, bedIndexCount);
	bedVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedVertexCount, GL_STATIC_DRAW, bedVertexData);
	bedIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedIndexCount, GL_STATIC_DRAW, bedIndexes);

	//���� ��--2 ���˴�+�Ҳ��̺
	bed2VertexData = LoadObjModel("Res/group2/bed2.obj", &bed2Indexes, bed2VertexCount, bed2IndexCount);
	bed2Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bed2VertexCount, GL_STATIC_DRAW, bed2VertexData);
	bed2Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bed2IndexCount, GL_STATIC_DRAW, bed2Indexes);

	//���� ����λ��--1 ��������
	window1VertexData = LoadObjModel("Res/group2/bedRoomCurtainOn.obj", &window1Indexes, window1VertexCount, window1IndexCount);
	window1Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * window1VertexCount, GL_STATIC_DRAW, window1VertexData);
	window1Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * window1IndexCount, GL_STATIC_DRAW, window1Indexes);

	//���� ����λ��--2 �����ر�
	window2VertexData = LoadObjModel("Res/group2/bedRoomCurtainOff.obj", &window2Indexes, window2VertexCount, window2IndexCount);
	window2Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * window2VertexCount, GL_STATIC_DRAW, window2VertexData);
	window2Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * window2IndexCount, GL_STATIC_DRAW, window2Indexes);

	//���� ����λ��--3 ��Ҷ������
	window3VertexData = LoadObjModel("Res/group2/bedRoomWindowOn.obj", &window3Indexes, window3VertexCount, window3IndexCount);
	window3Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * window3VertexCount, GL_STATIC_DRAW, window3VertexData);
	window3Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * window3IndexCount, GL_STATIC_DRAW, window3Indexes);

	//���� ����λ��--4 ��Ҷ������
	window4VertexData = LoadObjModel("Res/group2/bedRoomWindowOff.obj", &window4Indexes, window4VertexCount, window4IndexCount);
	window4Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * window4VertexCount, GL_STATIC_DRAW, window4VertexData);
	window4Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * window4IndexCount, GL_STATIC_DRAW, window4Indexes);

	//���� ǽ��װ��--1
	decoVertexData = LoadObjModel("Res/group2/bedroomonWall.obj", &decoIndexes, decoVertexCount, decoIndexCount);
	decoVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * decoVertexCount, GL_STATIC_DRAW, decoVertexData);
	decoIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * decoIndexCount, GL_STATIC_DRAW, decoIndexes);

	//���� ǽ��װ��2
	deco2VertexData = LoadObjModel("Res/group2/bedroomonWall2.obj", &deco2Indexes, deco2VertexCount, deco2IndexCount);
	deco2Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * deco2VertexCount, GL_STATIC_DRAW, deco2VertexData);
	deco2Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * deco2IndexCount, GL_STATIC_DRAW, deco2Indexes);

	//���� ������Ʒ
	bedroomVertexData = LoadObjModel("Res/group2/bedroom.obj", &bedroomIndexes, bedroomVertexCount, bedroomIndexCount);
	bedroomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedroomVertexCount, GL_STATIC_DRAW, bedroomVertexData);
	bedroomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedroomIndexCount, GL_STATIC_DRAW, bedroomIndexes);

	//�鷿��Ʒ
	studyRoomVertexData = LoadObjModel("Res/group3/studyRoom.obj", &studyRoomIndexes, studyRoomVertexCount, studyRoomIndexCount);
	studyRoomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * studyRoomVertexCount, GL_STATIC_DRAW, studyRoomVertexData);
	studyRoomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * studyRoomIndexCount, GL_STATIC_DRAW, studyRoomIndexes);

	//������Ʒ
	kitchenVertexData = LoadObjModel("Res/group3/kitchen.obj", &kitchenIndexes, kitchenVertexCount, kitchenIndexCount);
	kitchenVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * kitchenVertexCount, GL_STATIC_DRAW, kitchenVertexData);
	kitchenIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * kitchenIndexCount, GL_STATIC_DRAW, kitchenIndexes);
}

//��������
void loadTexture() {
	paintings.push_back(CreateTexture("Res/Colors.png"));
	paintings.push_back(CreateTexture("Res/paintings/paint1.png"));
	paintings.push_back(CreateTexture("Res/paintings/paint2.png"));
	paintings.push_back(CreateTexture("Res/paintings/paint3.png"));
	paintings.push_back(CreateTexture("Res/paintings/paint4.png"));
	paintingsSize = paintings.size();

	otherPaintings.push_back(CreateTexture("Res/Colors.png"));
	otherPaintings.push_back(CreateTexture("Res/paintings/paint1.png"));
	otherPaintings.push_back(CreateTexture("Res/paintings/paint2.png"));
	otherPaintings.push_back(CreateTexture("Res/paintings/paint3.png"));
	otherPaintings.push_back(CreateTexture("Res/paintings/paint4.png"));
	OtherPaintingsSize = otherPaintings.size();

	walls.push_back(CreateTexture("Res/Colors.png"));
	walls.push_back(CreateTexture("Res/walls/wall1.png"));
	walls.push_back(CreateTexture("Res/walls/wall2.png"));
	walls.push_back(CreateTexture("Res/walls/wall3.png"));
	walls.push_back(CreateTexture("Res/walls/wall4.png"));
	wallsSize = walls.size();

	floors.push_back(CreateTexture("Res/Colors.png"));
	floors.push_back(CreateTexture("Res/floors/Floor1.png"));
	floors.push_back(CreateTexture("Res/floors/Floor2.png"));
	floors.push_back(CreateTexture("Res/floors/Floor3.png"));
	floors.push_back(CreateTexture("Res/floors/Floor4.png"));
	floors.push_back(CreateTexture("Res/floors/Floor5.png"));
	floorsSize = floors.size();
}

void init()
{
	//��ȡģ��=============================================================================
	readModel();
	loadTexture();

	cameraPos = glm::vec3(0.0f, 10.0f, 40.0f); //��ʼ���������λ��(x,y,z)
	cameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);//���������
	up = glm::vec3(0.0f, 1.0f, 0.0f);//���������� ���ڲ��

	cameraDirection = glm::normalize(cameraTarget - cameraPos);//���������
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));//����
	cameraUp = glm::cross(cameraDirection, cameraRight);//����
	cameraSpeed = 2.5f;//����������ٶ�

	projectionMatrix = glm::perspective(60.0f, 800.0f / 600.0f, 0.1f, 800.0f);//ͶӰ����
	viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));//�۲����

	//ǰ��������ָ����ƽ��ͷ����������꣬�����͵��Ĳ���ָ����ƽ��ͷ��ĵײ��Ͷ�����
	//ͨ�����ĸ��������Ƕ����˽�ƽ���Զƽ��Ĵ�С��
	//Ȼ�����͵��������������˽�ƽ���Զƽ��ľ��롣
	lightProjectionMatrix = glm::ortho(-320.0f, 320.0f, -320.0f, 320.0f, 0.1f, 1000.0f);
	
	//��һ������Ϊ��׶������֮��ļнǣ��ڶ�������Ϊ��߱ȣ����Ӵ��Ŀ�/�ߣ��������ĸ������ֱ�Ϊ�������Զ��������
	lightViewMatrix = glm::lookAt(glm::vec3(150.0f, 150.0f, 100.0f), glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	renderProgram = CreateGPUProgram("Res/light.vs", "Res/light.fs");           // ����gpu program
	depthProgram = CreateGPUProgram("Res/sample.vs", "Res/sample.fs");

	renderPosLoc = 0;
	renderTexcoordLoc = 1;
	renderNormalLoc = 2;
	renderMLoc = glGetUniformLocation(renderProgram, "M");       //  ��ȡgpu program�в�����λ�ã���ЩgetUniformLocationֻ�ǵõ�ֵ��λ�ã���û��ʽ��ֵ
	renderVLoc = glGetUniformLocation(renderProgram, "V");
	renderPLoc = glGetUniformLocation(renderProgram, "P");
	renderNMLoc = glGetUniformLocation(renderProgram, "NM");
	renderViewPosLocation = glGetUniformLocation(renderProgram, "ViewPos");
	renderTextureLoc = glGetUniformLocation(renderProgram, "U_MainTexture");
	shadowMapLoc = glGetUniformLocation(renderProgram, "U_ShadowMap");
	renderLightVMatrixLoc = glGetUniformLocation(renderProgram, "LightViewMatrix");
	renderLightPMatrixLoc = glGetUniformLocation(renderProgram, "LightProjectionMatrix");
	renderLightOnLoc = glGetUniformLocation(renderProgram, "lightOn");

	rendersunlight = glGetUniformLocation(renderProgram, "sunlight");//��Դλ��

	roomTexture = CreateTexture("Res/Colors.png");										//��misc.cpp��
	
	depthPosLoc = 0;
	depthTexcoordLoc = 1;
	depthNormalLoc = 2;
	depthMLoc = glGetUniformLocation(depthProgram, "M");
	depthVLoc = glGetUniformLocation(depthProgram, "V");
	depthPLoc = glGetUniformLocation(depthProgram, "P");

	glEnable(GL_DEPTH_TEST);
	//Ҫ��ÿ����Ⱦ����֮ǰ�����Ȼ���glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//������FBO�������õ�FBO.h �� FBO.cpp
	mFbo = new FrameBufferObject;												//FBO.h
	mFbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, 800, 600);			//��һ��colorbuffer��800,600����Ļ��ߣ����롰color��
	mFbo->AttachDepthBuffer("depth", 800, 600);									//��һ��depthbuffer��800,600����Ļ��ߣ����롰depth��
	mFbo->Finish();

	mFbo->Bind();//�󶨵�ǰ��framebuffer�����������
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawRoomSample();
	mFbo->Unbind();

	shadowMap = mFbo->GetBuffer("depth");//�õ���Ȼ�����

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
}

void display()
{
	mFbo->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawRoomSample();               //  �򵥻���һ֡���õ���Ӱͼ

	mFbo->Unbind();
	shadowMap = mFbo->GetBuffer("depth");

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawRoom();         //  ����

	glutSwapBuffers();
	glutPostRedisplay();
}


void keyFunc(GLubyte key, int x, int y)          // ���̽���������   ws�ƶ������   c�л�������  l���ص�
{
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));//���ݳ���ı�����

	switch (key)
	{
		//������ƶ�------------------------------------------------------
	case 'w': case 'W'://ǰ��
		cameraPos += cameraSpeed * cameraDirection;
		cameraTarget += cameraSpeed * cameraDirection;
		break;
	case 's': case 'S'://����
		cameraPos -= cameraSpeed * cameraDirection;
		cameraTarget -= cameraSpeed * cameraDirection;
		break;
	case 'a': case 'A'://����
		cameraPos += cameraSpeed * cameraRight;
		cameraTarget += cameraSpeed * cameraRight;
		break;
	case 'd': case 'D'://����
		cameraPos -= cameraSpeed * cameraRight;
		cameraTarget -= cameraSpeed * cameraRight;
		break;
	case ' ': //����
		cameraPos += cameraSpeed * up;
		cameraTarget += cameraSpeed * up;
		break;
	case 'x'://�ݵ�
		cameraPos -= cameraSpeed * up;
		cameraTarget -= cameraSpeed * up;
		break;
	case 'k': case 'K':
		sunlight -= 0.5;
		//printf("%f", sunlight);
		if (sunlight <= 0) {
			sunlight = 0.5;
		}
		break;
	case 'j': case 'J':
		sunlight += 0.5;
		//if (sunlight >= 50.0) sunlight = 50.0;
		break;
	case 'h':case 'H':
		sunlight = 50.0;
		break;
		//��Ʒ��ʽ�л�------------------------------------------------------
	case '1'://�л����Ҵ���ʽ
		bed = (bed + 1) % 2;
		break;
	case '2'://�л�������ǽװ��
		wallDeco = (wallDeco + 1) % 2;
		break;
	case '3'://�л����Ҵ�����ʽ
		window = (window + 1) % 4;
		break;
	case '4'://�л�������ʽ
		drawer = (drawer + 1) % 2;
		break;
	case '0'://�л�����ǽ������
		wallTexture++;
		wallTexture %= wallsSize;
		break;
	case '9'://�л������ذ�����
		floorTexture++;
		floorTexture %= floorsSize;
		break;
	case '8'://�л������м�һ�����
		paintTexture++;
		paintTexture %= paintingsSize;
		break;
	case '7'://�л�������������
		otherPaintTexture++;
		otherPaintTexture %= OtherPaintingsSize;
		break;
		//���ص�------------------------------------------------------------
	case 'l': case 'L':
		lightOn = lightOn == 1 ? 0 : 1;
		break;
	}
	lightViewMatrix = glm::lookAt(glm::vec3(150.0f, 150.0f, 100.0f), glm::vec3(0.0f, 0.0f, -50.0f) + glm::vec3(sunlight, sunlight, 0), glm::vec3(0.0f, 0.0f, -1.0f));
	viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}

void MouseFunc(int button, int state, int x, int y)      // ��꺯����  �����Ҽ������ƶ�����ͷ�� �ɿ��Ҽ����������ƶ�
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		bCameraRotate = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		bCameraRotate = false;
	}
	//���ֿ�����Ұ�ķŴ����С
	if (button == 3)
	{
		fov += 0.3f;
	}
	if (button == 4) {
		fov -= 0.3f;
	}
}

void MotionFunc(int x, int y)       // ����ƶ�������  �Ҽ������ƶ����ڶ�����ͷ
{
	y = 600 - y;
	if (bCameraRotate)
	{
		if (x > lastMousePosX)
		{
			cameraXZRotateAngle += 0.02f;
			lastMousePosX = x;
		}
		else if (x < lastMousePosX)
		{
			cameraXZRotateAngle -= 0.02f;
			lastMousePosX = x;
		}
		if (y > lastMousePosY)
		{
			cameraYRotateAngle += 0.02f;
			lastMousePosY = y;
		}
		else if (y < lastMousePosY)
		{
			cameraYRotateAngle -= 0.02f;
			lastMousePosY = y;
		}
	}

	cameraDirection.x = sin(cameraXZRotateAngle);
	cameraDirection.z = -cos(cameraXZRotateAngle);
	cameraDirection.y = sin(cameraYRotateAngle);

	cameraDirection = glm::normalize(cameraDirection);

	cameraTarget = cameraPos + cameraDirection;

	viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("opengl");

	glewInit();

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);

	printf("===============���ι���================\n");
	printf("����w��s��d��aǰ�������ƶ�...\n");
	printf("����ո������ƶ�...\n");
	printf("����x�����ƶ�...\n");
	printf("ͨ�����ֿ��ƷŴ���С\n");
	printf("===============�л���״================\n");
	printf("����1�л���...\n");
	printf("����2�л�����ǽװ��...\n");
	printf("����3�л����Ҵ�����ʽ...\n");
	printf("����4�л�������ʽ...\n");
	printf("===============�л�����================\n");
	printf("����0�л�����ǽ����ʽ...\n");
	printf("����9�л������ذ���ʽ...\n");
	printf("����8�л������м�ڻ���ʽ...\n");
	printf("����7�л��������߱ڻ���ʽ...\n");
	printf("================�������===============\n");
	printf("����L���ص�...\n");
	printf("����k̫����ɽ...\n");
	printf("����j̫������...\n");
	printf("����h��������...\n");
	glutMainLoop();

	return 0;
}

/*
* @����������һ������
* @˵���������ʼ���������ɵ�vbo��ibo��indexcount�����Ƶ�������
*/
void drawOne(GLuint vbo, GLuint ibo, int indexCount) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//����opengl����ν����������ݣ����� ��һ��������vs��location��ֵ
	glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
	glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);        // ����ibo�е�index����ͼ��
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawOneSample(GLuint vbo, GLuint ibo, int indexCount) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//����opengl����ν����������ݣ����� ��һ��������vs��location��ֵ
	glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
	glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);        // ����ibo�е�index����ͼ��
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawRoom()
{
	//--------------------------------------------------------------------------------------------------------------
	//׼������
	//--------------------------------------------------------------------------------------------------------------
	modelMatrix = glm::scale(2.0f, 2.0f, 2.0f);
	normalMatrix = glm::inverseTranspose(modelMatrix); //ģ�;��󣬿���ƽ�ơ���ת������
	/**
	*void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
	*ͨ��һ�±�����uniform���εı��������ý�һ�±���ֵ������Ⱦ���ߡ�
	*location : uniform��λ�á�
	*count : ��Ҫ�������ݵ�����Ԫ�ص�����������Ҫ�޸ĵľ����������
	*transpose : ָ��������������(column major)����GL_FALSE������������(row major)����GL_TRUE����
	*value : ָ����count��Ԫ�ص������ָ��
	*/

	//����uniform֮ǰ�������ʹ�ó��򣨵���glUseProgram)����Ϊ�����ڵ�ǰ�������ɫ������������uniform�ġ�
	glUseProgram(renderProgram);
	//glUniformMatrix4fv����ɫ������ֵ����һ��������λ�ã�2��3���䣬���ĸ�������Ҫ���ľ���
	glUniformMatrix4fv(renderMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));             // ��gpu program�д�ֵ
	glUniformMatrix4fv(renderVLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));			//��Щ��������47�и�ֵ
	projectionMatrix = glm::perspective(fov, 800.0f / 600.0f, 0.1f, 800.0f);//ͶӰ����������޸���fov������������Ҫʵʱ���ݾ������ɫ������
	glUniformMatrix4fv(renderPLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(renderNMLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(renderViewPosLocation, 1, glm::value_ptr(cameraPos));
	//glUniform1i����ÿ���������ķ�ʽ��������OpenGLÿ����ɫ�������������ĸ�����Ԫ������Ԫ�ܹ���16��
	glUniformMatrix4fv(renderLightPMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));
	glUniformMatrix4fv(renderLightVMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniform1i(renderLightOnLoc, lightOn);

	//��Դλ�ô�ֵ
	glUniform1f(rendersunlight, sunlight);

	//������Ԫ
	glBindTextureUnit(0, roomTexture);      // ������Ԫ0
	glBindTextureUnit(1, shadowMap);

	glEnableVertexAttribArray(renderPosLoc);
	glEnableVertexAttribArray(renderTexcoordLoc);
	glEnableVertexAttribArray(renderNormalLoc);
	//--------------------------------------------------------------------------------------------------------------
	//��ʼ����
	//--------------------------------------------------------------------------------------------------------------
	//�ذ�
	glBindTextureUnit(0, floors[floorTexture]);      // ������Ԫ0
	drawOne(floorVbo, floorIbo, floorIndexCount);
	glBindTextureUnit(0, roomTexture);
	//ǽ��
	glBindTextureUnit(0, walls[wallTexture]);      // ������Ԫ0
	drawOne(wallVbo, wallIbo, wallIndexCount);
	glBindTextureUnit(0, roomTexture);

	//����
	drawOne(tableVbo, tableIbo, tableIndexCount);
	//������Ʒ
	drawOne(kitchenVbo, kitchenIbo, kitchenIndexCount);
	//�鷿��Ʒ
	drawOne(studyRoomVbo, studyRoomIbo, studyRoomIndexCount);
	//������Ʒ
	drawOne(roomVbo, roomIbo, roomIndexCount);
	//���� ǽ��--��1
	glBindTextureUnit(0, paintings[paintTexture]);
	drawOne(pVbo, pIbo, pIndexCount);
	glBindTextureUnit(0, roomTexture);
	//���� ǽ��--��2
	glBindTextureUnit(0, otherPaintings[otherPaintTexture]);
	drawOne(p2Vbo, p2Ibo, p2IndexCount);
	glBindTextureUnit(0, roomTexture);

	//���� ��
	if (bed == 0)
		drawOne(bedVbo, bedIbo, bedIndexCount);//��1
	else
		drawOne(bed2Vbo, bed2Ibo, bed2IndexCount);//��2
	//����
	if (drawer == 0)
		drawOne(drawerVbo, drawerIbo, drawerIndexCount);//����1
	else
		drawOne(drawerVbo2, drawerIbo2, drawerIndexCount2);//����2

	//���� ��ǽ--����
	switch (window) {
	case(0)://��ʽ1 �����ر�
		drawOne(window1Vbo, window1Ibo, window1IndexCount); break;
	case(1)://��ʽ2 ������
		drawOne(window2Vbo, window2Ibo, window2IndexCount); break;
	case(2)://��ʽ3 ��Ҷ������
		drawOne(window3Vbo, window3Ibo, window3IndexCount); break;
	case(3)://��ʽ4 ��Ҷ������
		drawOne(window4Vbo, window4Ibo, window4IndexCount); break;
	}

	//���� ��ǽ
	if (wallDeco == 0)
		drawOne(decoVbo, decoIbo, decoIndexCount);// װ��1 �յ�������
	else
		drawOne(deco2Vbo, deco2Ibo, deco2IndexCount); //װ��2 �鼮

	//���� ������Ʒ ��Զ�� ̨�� ��ͷ�� ����
	drawOne(bedroomVbo, bedroomIbo, bedroomIndexCount);
	glUseProgram(0);
	//--------------------------------------------------------------------------------------------------------------
	//���ƽ���
	//--------------------------------------------------------------------------------------------------------------
}

void DrawRoomSample()
{
	modelMatrix = glm::scale(2.0f, 2.0f, 2.0f);
	glUseProgram(depthProgram);
	glUniformMatrix4fv(depthMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(depthVLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));

	glEnableVertexAttribArray(depthPosLoc);
	glEnableVertexAttribArray(depthTexcoordLoc);
	glEnableVertexAttribArray(depthNormalLoc);

	//--------------------------------------------------------------------------------------------------------------
	//��ʼ����
	//--------------------------------------------------------------------------------------------------------------
	//����
	drawOneSample(tableVbo, tableIbo, tableIndexCount);

	//�ذ�
	drawOneSample(floorVbo, floorIbo, floorIndexCount);
	//ǽ��
	drawOneSample(wallVbo, wallIbo, wallIndexCount);
	//������Ʒ
	drawOneSample(kitchenVbo, kitchenIbo, kitchenIndexCount);
	//�鷿��Ʒ
	drawOneSample(studyRoomVbo, studyRoomIbo, studyRoomIndexCount);
	//������Ʒ
	drawOne(roomVbo, roomIbo, roomIndexCount);
	//���� ǽ��--��1
	drawOneSample(pVbo, pIbo, pIndexCount);
	//���� ǽ��--��2
	drawOneSample(p2Vbo, p2Ibo, p2IndexCount);

	//���� ��
	if (bed == 0)
		drawOneSample(bedVbo, bedIbo, bedIndexCount);//��1
	else
		drawOneSample(bed2Vbo, bed2Ibo, bed2IndexCount);//��2
	//����
	if (drawer == 0)
		drawOneSample(drawerVbo, drawerIbo, drawerIndexCount);//����1
	else
		drawOneSample(drawerVbo2, drawerIbo2, drawerIndexCount2);//����2
	//���� ��ǽ--����
	switch (window) {
	case(0)://��ʽ1 �����ر�
		drawOneSample(window1Vbo, window1Ibo, window1IndexCount); break;
	case(1)://��ʽ2 ������
		drawOneSample(window2Vbo, window2Ibo, window2IndexCount); break;
	case(2)://��ʽ3 ��Ҷ������
		drawOneSample(window3Vbo, window3Ibo, window3IndexCount); break;
	case(3)://��ʽ4 ��Ҷ������
		drawOneSample(window4Vbo, window4Ibo, window4IndexCount); break;
	}

	//���� ��ǽ
	if (wallDeco == 0)
		drawOneSample(decoVbo, decoIbo, decoIndexCount);// װ��1 �յ�������
	else
		drawOneSample(deco2Vbo, deco2Ibo, deco2IndexCount); //װ��2 �鼮

	//���� ������Ʒ ��Զ�� ̨�� ��ͷ�� ����
	drawOneSample(bedroomVbo, bedroomIbo, bedroomIndexCount);
	glUseProgram(0);
}