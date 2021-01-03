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

//bool bDrawRoom1 = true;  // ��һ��װ�޷���
int bDrawRoom1 = 0;
int wall1 = 0; //����ǽ�ڷ���
int wall2 = 0; //����ǽ�ڷ���
int wallDeco = 0;//����ǽ��װ��
int floor1 = 0;//���ҵذ�
int bed = 0;//���Ҵ���ʽ
int window = 0;//���Ҵ�����ʽ

int lightOn = 1;   // �Ƿ�������

void init()
{
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

	roomTexture = CreateTexture("Res/Colors.png");										//��misc.cpp��
	//roomTexture = CreateTexture("Res/Floor4.png");
	depthPosLoc = 0;
	depthTexcoordLoc = 1;
	depthNormalLoc = 2;
	depthMLoc = glGetUniformLocation(depthProgram, "M");
	depthVLoc = glGetUniformLocation(depthProgram, "V");
	depthPLoc = glGetUniformLocation(depthProgram, "P");

	//��������
	roomVertexData2 = LoadObjModel("Res/room.obj", &roomIndexes2, roomVertexCount2, roomIndexCount2);
	roomVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount2, GL_STATIC_DRAW, roomVertexData2);
	roomIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount2, GL_STATIC_DRAW, roomIndexes2);

	//����ǽ�ڵ�VBO��VIO������ֻ�ǳ�ʼ��
	wall_yellow = LoadObjModel("Res/livingRoomWall_Yellow.obj", &wallIndexes, wallVertexCount, wallIndexCount);
	wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_yellow);
	//printf("��ʼǽ��һ\n");
	wallIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * wallIndexCount, GL_STATIC_DRAW, wallIndexes);

	//�����ڲ�
	roomVertexData = LoadObjModel("Res/livingRoom.obj", &roomIndexes, roomVertexCount, roomIndexCount);
	roomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount, GL_STATIC_DRAW, roomVertexData);
	roomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount, GL_STATIC_DRAW, roomIndexes);

	//����ǽ��
	VertexData* wall_blue1 = LoadObjModel("Res/roomWall_Blue.obj", &wallIndexes2, wallVertexCount2, wallIndexCount2);
	wallVertexData2 = wall_blue1;//��ʼ����
	wallVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount2, GL_STATIC_DRAW, wall_blue1);//��ʼ��ɫ
	wallIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * wallIndexCount2, GL_STATIC_DRAW, wallIndexes2);

	//����VBO��VIO�����initֻ��ִ��һ�Σ��ı��ڰ�����������
	bedVertexData = LoadObjModel("Res/bed.obj", &bedIndexes, bedVertexCount, bedIndexCount);
	bedVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedVertexCount, GL_STATIC_DRAW, bedVertexData);
	bedIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedIndexCount, GL_STATIC_DRAW, bedIndexes);
	//printf("��ʼ��һ\n");

	//����
	curtainVertexData = LoadObjModel("Res/curtain.obj", &curtainIndexes, curtainVertexCount, curtainIndexCount);
	curtainVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * curtainVertexCount, GL_STATIC_DRAW, curtainVertexData);
	curtainIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * curtainIndexCount, GL_STATIC_DRAW, curtainIndexes);

	//�ذ�
	floorVertexData = LoadObjModel("Res/floor.obj", &floorIndexes, floorVertexCount, floorIndexCount);
	floorVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * floorVertexCount, GL_STATIC_DRAW, floorVertexData);
	floorIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * floorIndexCount, GL_STATIC_DRAW, floorIndexes);

	//װ��
	decoVertexData = LoadObjModel("Res/onWall.obj", &decoIndexes, decoVertexCount, decoIndexCount);
	decoVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * decoVertexCount, GL_STATIC_DRAW, decoVertexData);
	decoIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * decoIndexCount, GL_STATIC_DRAW, decoIndexes);

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
		//������ƶ�
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
	case 'c': case 'C'://����ǽ����ɫ�л�
		//printf("�л�����ǽ��\n");
		changeWall++;
		changeWall = changeWall % 5;
		switch (changeWall) {
		case 0:
			wall_yellow = LoadObjModel("Res/livingRoomWall_Yellow.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_yellow);
			//printf("�л�ǽ��һ\n");
			break;
		case 1:
			wall_green = LoadObjModel("Res/livingRoomWall_Green.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_green);
			//printf("�л�ǽ�ڶ�\n");
			break;
		case 2:
			wall_white = LoadObjModel("Res/livingRoomWall_White.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_white);
			//printf("�л�ǽ����\n");
			break;
		case 3:
			wall_orange = LoadObjModel("Res/livingRoomWall_Orange.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_orange);
			//printf("�л�ǽ����\n");
			break;
		case 4:
			wall_blue = LoadObjModel("Res/livingRoomWall_Blue.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_blue);
			//printf("�л�ǽ����\n");
			break;
		}
		wallIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * wallIndexCount, GL_STATIC_DRAW, wallIndexes);
		//printf("changeWall=%d", changeWall);
		break;
	case 'v':case 'V'://�����л�
		//printf("�л���\n");
		changeBed++;
		changeBed = changeBed % 2;
		switch (changeBed) {
		case 0:
			bedVertexData = LoadObjModel("Res/bed.obj", &bedIndexes, bedVertexCount, bedIndexCount);
			bedVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedVertexCount, GL_STATIC_DRAW, bedVertexData);
			bedIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedIndexCount, GL_STATIC_DRAW, bedIndexes);
			//printf("��һ\n");
			break;
		case 1:
			bedVertexData = LoadObjModel("Res/bed2.obj", &bedIndexes, bedVertexCount, bedIndexCount);
			bedVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedVertexCount, GL_STATIC_DRAW, bedVertexData);
			bedIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedIndexCount, GL_STATIC_DRAW, bedIndexes);
			//printf("����\n");
			break;
		}
		//printf("changeBed=%d", changeBed);	
		break;
	case 'b':case 'B'://�������л�
		//printf("�л�����\n");
		changeCurtain++;
		changeCurtain = changeCurtain % 2;
		switch (changeCurtain) {
		case 0:
			curtainVertexData = LoadObjModel("Res/curtain.obj", &curtainIndexes, curtainVertexCount, curtainIndexCount);
			curtainVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * curtainVertexCount, GL_STATIC_DRAW, curtainVertexData);
			curtainIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * curtainIndexCount, GL_STATIC_DRAW, curtainIndexes);
			printf("����1...\n");
			break;
		case 1:
			curtainVertexData = LoadObjModel("Res/window.obj", &curtainIndexes, curtainVertexCount, curtainIndexCount);
			curtainVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * curtainVertexCount, GL_STATIC_DRAW, curtainVertexData);
			curtainIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * curtainIndexCount, GL_STATIC_DRAW, curtainIndexes);
			printf("����2...\n");
			break;
		}
		//printf("changeCurtain=%d", changeCurtain);
		
		break;
	case 'l': case 'L':
		lightOn = lightOn == 1 ? 0 : 1;
		break;
	case 't':case 'T':
		changeStyle++;
		changeStyle = changeStyle % 3;
		//printf("%d\n", changeStyle);
		switch (changeStyle) {
		case 0:
			//printf("room1...\n");
			roomVertexData2 = LoadObjModel("Res/room.obj", &roomIndexes2, roomVertexCount2, roomIndexCount2);
			roomVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount2, GL_STATIC_DRAW, roomVertexData2);
			roomIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount2, GL_STATIC_DRAW, roomIndexes2);
			break;
		case 1:
			//printf("room2...\n");
			roomVertexData2 = LoadObjModel("Res/room2.obj", &roomIndexes2, roomVertexCount2, roomIndexCount2);
			roomVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount2, GL_STATIC_DRAW, roomVertexData2);
			roomIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount2, GL_STATIC_DRAW, roomIndexes2);
			break;
		}
		break;
	}
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

	printf("=============���̽���=============\n");
	printf("���� w��s��d��a ǰ�������ƶ�...\n");
	printf("���� �ո� �����ƶ�...\n");
	printf("���� x �����ƶ�...\n");
	printf("���� c �л�ǽ�ڿ���...\n");
	printf("���� v �л���...\n");
	printf("���� l ���ص�...\n");
	printf("���� b �л�����...\n");
	printf("���� t �л�ģ��...\n");
	printf("=============��꽻��=============\n");
	printf("�϶�����Ҽ��ӽ��л�...\n");
	printf("��������Ұ�Ŵ���С...\n");
	glutMainLoop();

	return 0;
}

void DrawRoom()
{
	modelMatrix = glm::scale(2.0f, 2.0f, 2.0f);
	normalMatrix = glm::inverseTranspose(modelMatrix);

	/**
	*void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
	*ͨ��һ�±�����uniform���εı��������ý�һ�±���ֵ������Ⱦ���ߡ�
	*location : uniform��λ�á�
	*count : ��Ҫ�������ݵ�����Ԫ�ص�����������Ҫ�޸ĵľ����������
	*transpose : ָ��������������(column major)����GL_FALSE������������(row major)����GL_TRUE����
	*value : ָ����count��Ԫ�ص������ָ��
	*/
	//��Simple.vs�е�Posλ��������Normal������,Texcoord������������
	glUseProgram(renderProgram);
	glUniformMatrix4fv(renderMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));             // ��gpu program�д�ֵ
	glUniformMatrix4fv(renderVLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));			//��Щ��������53�л��λ�þ����Ȼ�������︳ֵ
	projectionMatrix = glm::perspective(fov, 800.0f / 600.0f, 0.1f, 800.0f);//ͶӰ����������޸���fov������������Ҫʵʱ���ݾ������ɫ������
	glUniformMatrix4fv(renderPLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(renderNMLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(renderViewPosLocation, 1, glm::value_ptr(cameraPos));
	glUniformMatrix4fv(renderLightPMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));
	glUniformMatrix4fv(renderLightVMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniform1i(renderLightOnLoc, lightOn);

	glBindTextureUnit(0, roomTexture);      // ����������Light.fs��binding = 0 ��1
	glBindTextureUnit(1, shadowMap);//��Ȼ�����ȥ�������ж��Ƿ�����Ӱ��

	if (changeStyle == 2) {
		//���ƿ���===============================================================
		glEnableVertexAttribArray(renderPosLoc);//light.fs λ������
		glEnableVertexAttribArray(renderTexcoordLoc);//light.fs ��������
		glEnableVertexAttribArray(renderNormalLoc);//light.fs ����������

		//��ʼ���ƿ����ڲ�------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, roomIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//��ʼ���ƿ���ǽ��------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, wallVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, wallIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//���ƴ���---------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, curtainVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curtainIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, curtainIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//���Ƶذ�---------------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, floorIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//����װ��---------------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, decoVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, decoIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, decoIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//���ƴ�
		glBindBuffer(GL_ARRAY_BUFFER, bedVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bedIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, bedIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//��������ǽ��
		glBindBuffer(GL_ARRAY_BUFFER, wallVbo2);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIbo2);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, wallIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo2);
		glEnableVertexAttribArray(renderPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(renderTexcoordLoc);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(renderNormalLoc);
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo2);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, roomIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glUseProgram(0);
}

void DrawRoomSample()
{
	modelMatrix = glm::scale(2.0f, 2.0f, 2.0f);
	glUseProgram(depthProgram);
	//uniform��λ�ã���Ҫ�������ݵ�����Ԫ�ص�����������Ҫ�޸ĵľ����������ָ��������������(column major)����GL_FALSE������������(row major)����GL_TRUE��ָ����count��Ԫ�ص������ָ�롣
	glUniformMatrix4fv(depthMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(depthVLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));

	if (changeStyle == 2) {
		//��ʼ���ƿ����ڲ�------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, roomIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//��ʼ���ƿ���ǽ��------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, wallVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, wallIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//���ƴ���---------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, curtainVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curtainIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, curtainIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//���Ƶذ�---------------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, floorIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//����װ��---------------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, decoVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, decoIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, decoIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//���ƴ�
		glBindBuffer(GL_ARRAY_BUFFER, bedVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bedIbo);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, bedIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//��������ǽ��
		glBindBuffer(GL_ARRAY_BUFFER, wallVbo2);
		glEnableVertexAttribArray(depthPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIbo2);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, wallIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo2);
		glEnableVertexAttribArray(renderPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(renderTexcoordLoc);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(renderNormalLoc);
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo2);        // ����ibo�е�index����ͼ��
		glDrawElements(GL_TRIANGLES, roomIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/**
	* Ϊ���������ܣ���ʡgpu��Դ�ĺ��ã�����ʹ�ñ����޳�����
	//���ñ����޳�����	
	glEnable(GL_CULL_FACE);
	//�޳�����Ϊ����
	glCullFace(GL_BACK);
	//����Ļ���˳��Ϊ˳ʱ��
	glFrontFace(GL_CW);
	*/
	glUseProgram(0);
}