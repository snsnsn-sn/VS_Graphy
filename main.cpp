#include"misc.h"
#include"FBO.h"
#include"Param.h"
#pragma comment(lib, "soil.lib")
#pragma comment(lib, "glew32.lib")

FrameBufferObject* mFbo;

bool bCameraRotate = false;   //  摄像机是否允许旋转
float cameraXZRotateAngle = 0;   // 摄像机xz平面内的旋转角度 即左右旋转
float cameraYRotateAngle = 0;   //  摄像机垂直平面内的旋转角度   上下旋转

int lastMousePosX, lastMousePosY;  //  上次鼠标的位置  

void DrawRoom();
void DrawRoomSample();

bool bDrawRoom1 = true;  // 第一套装修方案
int lightOn = 1;   // 是否开启光照

void init()
{
	cameraPos = glm::vec3(0.0f, 10.0f, 40.0f);						//摄像机的位置
	cameraCenter = glm::vec3(0.0f, 10.0f, 0.0f);							
	//摄像机方向
	//cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);					//摄像机的指向，Target 就是 Center
	cameraDirection = glm::normalize(cameraTarget - cameraPos);		//让摄像机初始指向Z轴，代表摄像机的Z轴方向，Direction 就是 Forward
	//右轴
	up = glm::vec3(0.0f, 1.0f, 0.0f);								//先定义一个上向量，让他和Z轴叉乘得到右向量
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	//上轴
	cameraUp = glm::cross(cameraDirection, cameraRight);			//右向量和方向向量叉乘，up X cameraDirection = cameraRight；cameraRight X cameraDirection = cameraUp
	//
	cameraForward = glm::normalize(cameraCenter - cameraPos);

	projectionMatrix = glm::perspective(60.0f, 800.0f / 600.0f, 0.1f, 800.0f);
	viewMatrix = glm::lookAt(cameraPos, cameraCenter, glm::vec3(0.0f, 1.0f, 0.0f));

	//前两个参数指定了平截头体的左右坐标，第三和第四参数指定了平截头体的底部和顶部。
	//通过这四个参数我们定义了近平面和远平面的大小，
	//然后第五和第六个参数则定义了近平面和远平面的距离。
	lightProjectionMatrix = glm::ortho(-320.0f, 320.0f, -320.0f, 320.0f, 0.1f, 1000.0f);
	
	//第一个参数为视锥上下面之间的夹角，第二个参数为宽高比，即视窗的宽/高，第三第四个参数分别为近截面和远界面的深度
	lightViewMatrix = glm::lookAt(glm::vec3(150.0f, 150.0f, 100.0f), glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	renderProgram = CreateGPUProgram("Res/light.vs", "Res/light.fs");           // 创建gpu program
	depthProgram = CreateGPUProgram("Res/sample.vs", "Res/sample.fs");
	renderPosLoc = 0;
	renderTexcoordLoc = 1;
	renderNormalLoc = 2;
	renderMLoc = glGetUniformLocation(renderProgram, "M");       //  获取gpu program中参数的位置
	renderVLoc = glGetUniformLocation(renderProgram, "V");
	renderPLoc = glGetUniformLocation(renderProgram, "P");
	renderNMLoc = glGetUniformLocation(renderProgram, "NM");
	renderViewPosLocation = glGetUniformLocation(renderProgram, "ViewPos");
	renderTextureLoc = glGetUniformLocation(renderProgram, "U_MainTexture");
	shadowMapLoc = glGetUniformLocation(renderProgram, "U_ShadowMap");
	renderLightVMatrixLoc = glGetUniformLocation(renderProgram, "LightViewMatrix");
	renderLightPMatrixLoc = glGetUniformLocation(renderProgram, "LightProjectionMatrix");
	renderLightOnLoc = glGetUniformLocation(renderProgram, "lightOn");

	roomTexture = CreateTexture("Res/Colors.png");										//在misc.cpp中
	//roomTexture = CreateTexture("Res/Floor4.png");
	depthPosLoc = 0;
	depthTexcoordLoc = 1;
	depthNormalLoc = 2;
	depthMLoc = glGetUniformLocation(depthProgram, "M");
	depthVLoc = glGetUniformLocation(depthProgram, "V");
	depthPLoc = glGetUniformLocation(depthProgram, "P");
	//第一个房间模型
	roomVertexData = LoadObjModel("Res/room.obj", &roomIndexes, roomVertexCount, roomIndexCount);	//顶点的索引信息，顶点个数，索引个数

	roomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount,
		GL_STATIC_DRAW, roomVertexData);
	roomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount,	GL_STATIC_DRAW, roomIndexes);	   // 创建vbo  ibo
	//第二个房间模型
	roomVertexData2 = LoadObjModel("Res/room2.obj", &roomIndexes2, roomVertexCount2, roomIndexCount2);

	roomVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount2,
		GL_STATIC_DRAW, roomVertexData2);
	roomIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount2, GL_STATIC_DRAW, roomIndexes2);

	glEnable(GL_DEPTH_TEST);

	//以下是FBO技术，用到FBO.h 和 FBO.cpp
	mFbo = new FrameBufferObject;												//FBO.h
	mFbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, 800, 600);			//绑定一个colorbuffer，800,600是屏幕宽高
	mFbo->AttachDepthBuffer("depth", 800, 600);									//绑定一个depthbuffer，800,600是屏幕宽高
	mFbo->Finish();

	mFbo->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawRoomSample();
	mFbo->Unbind();

	shadowMap = mFbo->GetBuffer("depth");

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
}

void display()
{
	mFbo->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawRoomSample();               //  简单绘制一帧，得到阴影图

	mFbo->Unbind();
	shadowMap = mFbo->GetBuffer("depth");

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawRoom();         //  绘制

	glutSwapBuffers();
	glutPostRedisplay();
}


void keyFunc(GLubyte key, int x, int y)          // 键盘交互函数，   ws移动摄像机   c切换方案，  l开关灯
{
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));//根据朝向改变右轴

	switch (key)
	{
	case 'w': case 'W':
		cameraPos += cameraForward * cameraSpeed;
		cameraCenter += cameraForward * cameraSpeed;
		break;
	case 's': case 'S'://后移
		cameraPos -= cameraSpeed * cameraForward;
		cameraCenter -= cameraSpeed * cameraForward;
		break;
	case 'a': case 'A'://左移
		cameraPos += cameraSpeed * cameraRight;
		cameraCenter += cameraSpeed * cameraRight;
		break;
	case 'd': case 'D'://右移
		cameraPos -= cameraSpeed * cameraRight;
		cameraCenter -= cameraSpeed * cameraRight;
		break;
	case ' ': //飞天
		cameraPos += cameraSpeed * up;
		cameraCenter += cameraSpeed * up;
		break;
	case 'x'://遁地
		cameraPos -= cameraSpeed * up;
		cameraCenter -= cameraSpeed * up;
		break;
	case 'c': case 'C':
		bDrawRoom1 = !bDrawRoom1;
		break;

	case 'l': case 'L':
		lightOn = lightOn == 1 ? 0 : 1;
		break;
	}

	viewMatrix = glm::lookAt(cameraPos, cameraCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	//viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}

void MouseFunc(int button, int state, int x, int y)      // 鼠标函数，  单击右键允许移动摄像头， 松开右键即不允许移动
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

void MotionFunc(int x, int y)       // 鼠标移动函数，  右键摁下移动即摆动摄像头
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
	*通过一致变量（uniform修饰的变量）引用将一致变量值传入渲染管线。
	*location : uniform的位置。
	*count : 需要加载数据的数组元素的数量或者需要修改的矩阵的数量。
	*transpose : 指明矩阵是列优先(column major)矩阵（GL_FALSE）还是行优先(row major)矩阵（GL_TRUE）。
	*value : 指向由count个元素的数组的指针
	*/
	glUseProgram(renderProgram);
	glUniformMatrix4fv(renderMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));             // 向gpu program中传值
	glUniformMatrix4fv(renderVLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));			//这些参数都在47行赋值
	glUniformMatrix4fv(renderPLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(renderNMLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(renderViewPosLocation, 1, glm::value_ptr(cameraPos));
	glUniformMatrix4fv(renderLightPMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));
	glUniformMatrix4fv(renderLightVMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniform1i(renderLightOnLoc, lightOn);

	glBindTextureUnit(0, roomTexture);      // 绑定纹理
	glBindTextureUnit(1, shadowMap);

	if (bDrawRoom1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);              
		glEnableVertexAttribArray(renderPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(renderTexcoordLoc);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(renderNormalLoc);
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, roomIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
	glUniformMatrix4fv(depthMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(depthVLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));

	if (bDrawRoom1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);
		glEnableVertexAttribArray(depthPosLoc);
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

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
		* 为了提升性能，节省gpu资源的耗用，可以使用背面剔除功能
		//启用背面剔除功能	
		glEnable(GL_CULL_FACE);
		//剔除的面为背面
		glCullFace(GL_BACK);
		//顶点的绘制顺序为顺时针
		glFrontFace(GL_CW);
		*/
	}

	glUseProgram(0);
}