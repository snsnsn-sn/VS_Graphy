#include"misc.h"
#include"FBO.h"
#include"Param.h"
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

bool bDrawRoom1 = true;  // ��һ��װ�޷���
int lightOn = 1;   // �Ƿ�������

void init()
{
	cameraPos = glm::vec3(0.0f, 10.0f, 40.0f);						//�������λ��
	cameraCenter = glm::vec3(0.0f, 10.0f, 0.0f);							
	//���������
	//cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);					//�������ָ��Target ���� Center
	cameraDirection = glm::normalize(cameraTarget - cameraPos);		//���������ʼָ��Z�ᣬ�����������Z�᷽��Direction ���� Forward
	//����
	up = glm::vec3(0.0f, 1.0f, 0.0f);								//�ȶ���һ����������������Z���˵õ�������
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	//����
	cameraUp = glm::cross(cameraDirection, cameraRight);			//�������ͷ���������ˣ�up X cameraDirection = cameraRight��cameraRight X cameraDirection = cameraUp
	//
	cameraForward = glm::normalize(cameraCenter - cameraPos);

	projectionMatrix = glm::perspective(60.0f, 800.0f / 600.0f, 0.1f, 800.0f);
	viewMatrix = glm::lookAt(cameraPos, cameraCenter, glm::vec3(0.0f, 1.0f, 0.0f));

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
	renderMLoc = glGetUniformLocation(renderProgram, "M");       //  ��ȡgpu program�в�����λ��
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
	//��һ������ģ��
	roomVertexData = LoadObjModel("Res/room.obj", &roomIndexes, roomVertexCount, roomIndexCount);	//�����������Ϣ�������������������
	//VBO���ǰ�Ҫ���ƵĶ�����Ϣֱ�ӻ������Կ��ڴ���,VBOΪ���㻺�����������ڴ洢��������/����uv/���㷨��/������ɫ��
	roomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount,
		GL_STATIC_DRAW, roomVertexData);//����vbo���棬��һ���������������ͣ��ڶ����Ƕ���������������ǣ����ĸ��Ƕ�����Ϣ
	//IBOΪ�����������������ֵ������unsigned int����unsigned short��
	roomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount,	GL_STATIC_DRAW, roomIndexes);	   // ����vbo  ibo
	//�ڶ�������ģ��
	roomVertexData2 = LoadObjModel("Res/room2.obj", &roomIndexes2, roomVertexCount2, roomIndexCount2);

	roomVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount2,
		GL_STATIC_DRAW, roomVertexData2);
	roomIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount2, GL_STATIC_DRAW, roomIndexes2);

	glEnable(GL_DEPTH_TEST);

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
	case 'w': case 'W':
		cameraPos += cameraForward * cameraSpeed;
		cameraCenter += cameraForward * cameraSpeed;
		break;
	case 's': case 'S'://����
		cameraPos -= cameraSpeed * cameraForward;
		cameraCenter -= cameraSpeed * cameraForward;
		break;
	case 'a': case 'A'://����
		cameraPos += cameraSpeed * cameraRight;
		cameraCenter += cameraSpeed * cameraRight;
		break;
	case 'd': case 'D'://����
		cameraPos -= cameraSpeed * cameraRight;
		cameraCenter -= cameraSpeed * cameraRight;
		break;
	case ' ': //����
		cameraPos += cameraSpeed * up;
		cameraCenter += cameraSpeed * up;
		break;
	case 'x'://�ݵ�
		cameraPos -= cameraSpeed * up;
		cameraCenter -= cameraSpeed * up;
		break;
	case 'c': case 'C':
		bDrawRoom1 = !bDrawRoom1;
		break;

	case 'l': case 'L':
		lightOn = lightOn == 1 ? 0 : 1;
		break;
	
	/*case 'm': case 'M':
		changecolor += 0.5;
		if (changecolor > 2) changecolor = 0;
		break;*/
	}

	viewMatrix = glm::lookAt(cameraPos, cameraCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	//viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
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

	cameraForward.x = sin(cameraXZRotateAngle);
	cameraForward.z = -cos(cameraXZRotateAngle);
	cameraForward.y = sin(cameraYRotateAngle);

	cameraForward = glm::normalize(cameraForward);

	cameraCenter = cameraPos + cameraForward;

	viewMatrix = glm::lookAt(cameraPos, cameraCenter, glm::vec3(0.0f, 1.0f, 0.0f));
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
	glUniformMatrix4fv(renderVLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));			//��Щ��������47�и�ֵ
	glUniformMatrix4fv(renderPLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(renderNMLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(renderViewPosLocation, 1, glm::value_ptr(cameraPos));
	glUniformMatrix4fv(renderLightPMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));
	glUniformMatrix4fv(renderLightVMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniform1i(renderLightOnLoc, lightOn);

	glBindTextureUnit(0, roomTexture);      // ����������Light.fs��binding = 0 ��1
	glBindTextureUnit(1, shadowMap);//��Ȼ�����ȥ�������ж��Ƿ�����Ӱ��

	if (bDrawRoom1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);              
		glEnableVertexAttribArray(renderPosLoc);                 //  ����vbo ��gpu program�д�ֵ
		//����λ������
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(renderTexcoordLoc);
		//������������
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(renderNormalLoc);
		//������ɫ����
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo);        // ����ibo�е�index����ͼ��
		/**   void glDrawElements(GLenum mode,GLsizei count,GLenum type,const GLvoid *indices);
		*mode:���ܵ�ֵ����glBegin()�н��ܵ�ֵһ����������GL_POLYGON��GL_TRIANGLES��GL_TRIANGLE_STRIP��GL_LINE_STRIP�ȡ�
		*count����ϼ���ͼ�ε�Ԫ�صĸ�����һ���ǵ�ĸ�����
		*type:indeices������������ͣ���Ȼ��������һ�������͵ġ�
		*indices:��������
		*/
		glDrawElements(GL_TRIANGLES, roomIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//����ǰ��ڴ�����Ϊ�գ��Ǳ���
	}
	else if (!bDrawRoom1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo2);
		glEnableVertexAttribArray(renderPosLoc);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(renderTexcoordLoc);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(renderNormalLoc);
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo2);
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

	if (bDrawRoom1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);
		
		glEnableVertexAttribArray(depthPosLoc);//glEnableVertexAttribArray(index)����indexָ����ͨ�ö�����������
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);//�ڶ�������ָ���������ԵĴ�С������������һ��vec3������3��ֵ��ɣ����Դ�С��3��
		//����vs��layout=0,1,2��λ��
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		
		glEnableVertexAttribArray(depthNormalLoc);//���ĸ��������������Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣���ǰ�������ΪGL_FALSE��
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));
		//�����������������(Stride)�������������������Ķ���������֮��ļ���������¸���λ��������1��float֮�����ǰѲ�������Ϊ1 * sizeof(float)

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo);
		glDrawElements(GL_TRIANGLES, roomIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else if (!bDrawRoom1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo2);
		glEnableVertexAttribArray(depthPosLoc);
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo2);
		glDrawElements(GL_TRIANGLES, roomIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		/**
		* Ϊ���������ܣ���ʡgpu��Դ�ĺ��ã�����ʹ�ñ����޳�����
		//���ñ����޳�����	
		glEnable(GL_CULL_FACE);
		//�޳�����Ϊ����
		glCullFace(GL_BACK);
		//����Ļ���˳��Ϊ˳ʱ��
		glFrontFace(GL_CW);
		*/
	}

	glUseProgram(0);
}