#include"misc.h"
#include"FBO.h"
#include"Param.h"
#include<gl/freeglut.h>
#pragma comment(lib, "soil.lib")
#pragma comment(lib, "glew32.lib")

//冯氏光照模型(Phong Lighting Model)->由环境(Ambient)、漫反射(Diffuse)和镜面(Specular)光照三个分量组成
FrameBufferObject* mFbo;

bool bCameraRotate = false;   //  摄像机是否允许旋转
float cameraXZRotateAngle = 0;   // 摄像机xz平面内的旋转角度 即左右旋转
float cameraYRotateAngle = 0;   //  摄像机垂直平面内的旋转角度   上下旋转

int lastMousePosX, lastMousePosY;  //  上次鼠标的位置  

void DrawRoom();
void DrawRoomSample();

//bool bDrawRoom1 = true;  // 第一套装修方案
int bDrawRoom1 = 0;
int wall1 = 0; //客厅墙壁方案
int wall2 = 0; //卧室墙壁方案
int wallDeco = 0;//卧室墙壁装饰
int floor1 = 0;//卧室地板
int bed = 0;//卧室床样式
int window = 0;//卧室窗户样式

int lightOn = 1;   // 是否开启光照

void init()
{
	cameraPos = glm::vec3(0.0f, 10.0f, 40.0f); //初始化摄像机的位置(x,y,z)
	cameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);//摄像机朝向
	up = glm::vec3(0.0f, 1.0f, 0.0f);//定义上向量 用于叉乘

	cameraDirection = glm::normalize(cameraTarget - cameraPos);//摄像机朝向
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));//右轴
	cameraUp = glm::cross(cameraDirection, cameraRight);//上轴
	cameraSpeed = 2.5f;//设置摄像机速度

	projectionMatrix = glm::perspective(60.0f, 800.0f / 600.0f, 0.1f, 800.0f);//投影矩阵
	viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));//观察矩阵

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
	renderMLoc = glGetUniformLocation(renderProgram, "M");       //  获取gpu program中参数的位置，这些getUniformLocation只是得到值的位置，还没正式赋值
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

	//整个房间
	roomVertexData2 = LoadObjModel("Res/room.obj", &roomIndexes2, roomVertexCount2, roomIndexCount2);
	roomVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount2, GL_STATIC_DRAW, roomVertexData2);
	roomIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount2, GL_STATIC_DRAW, roomIndexes2);

	//客厅墙壁的VBO和VIO，这里只是初始化
	wall_yellow = LoadObjModel("Res/livingRoomWall_Yellow.obj", &wallIndexes, wallVertexCount, wallIndexCount);
	wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_yellow);
	//printf("初始墙壁一\n");
	wallIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * wallIndexCount, GL_STATIC_DRAW, wallIndexes);

	//客厅内部
	roomVertexData = LoadObjModel("Res/livingRoom.obj", &roomIndexes, roomVertexCount, roomIndexCount);
	roomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount, GL_STATIC_DRAW, roomVertexData);
	roomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount, GL_STATIC_DRAW, roomIndexes);

	//卧室墙壁
	VertexData* wall_blue1 = LoadObjModel("Res/roomWall_Blue.obj", &wallIndexes2, wallVertexCount2, wallIndexCount2);
	wallVertexData2 = wall_blue1;//初始数据
	wallVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount2, GL_STATIC_DRAW, wall_blue1);//初始颜色
	wallIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * wallIndexCount2, GL_STATIC_DRAW, wallIndexes2);

	//床的VBO，VIO，这个init只是执行一次，改变在按键函数那里
	bedVertexData = LoadObjModel("Res/bed.obj", &bedIndexes, bedVertexCount, bedIndexCount);
	bedVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedVertexCount, GL_STATIC_DRAW, bedVertexData);
	bedIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedIndexCount, GL_STATIC_DRAW, bedIndexes);
	//printf("初始床一\n");

	//窗帘
	curtainVertexData = LoadObjModel("Res/curtain.obj", &curtainIndexes, curtainVertexCount, curtainIndexCount);
	curtainVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * curtainVertexCount, GL_STATIC_DRAW, curtainVertexData);
	curtainIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * curtainIndexCount, GL_STATIC_DRAW, curtainIndexes);

	//地板
	floorVertexData = LoadObjModel("Res/floor.obj", &floorIndexes, floorVertexCount, floorIndexCount);
	floorVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * floorVertexCount, GL_STATIC_DRAW, floorVertexData);
	floorIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * floorIndexCount, GL_STATIC_DRAW, floorIndexes);

	//装饰
	decoVertexData = LoadObjModel("Res/onWall.obj", &decoIndexes, decoVertexCount, decoIndexCount);
	decoVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * decoVertexCount, GL_STATIC_DRAW, decoVertexData);
	decoIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * decoIndexCount, GL_STATIC_DRAW, decoIndexes);

	glEnable(GL_DEPTH_TEST);
	//要在每次渲染迭代之前清除深度缓存glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//以下是FBO技术，用到FBO.h 和 FBO.cpp
	mFbo = new FrameBufferObject;												//FBO.h
	mFbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, 800, 600);			//绑定一个colorbuffer，800,600是屏幕宽高，存入“color”
	mFbo->AttachDepthBuffer("depth", 800, 600);									//绑定一个depthbuffer，800,600是屏幕宽高，存入“depth”
	mFbo->Finish();

	mFbo->Bind();//绑定当前的framebuffer，清除缓冲区
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawRoomSample();
	mFbo->Unbind();

	shadowMap = mFbo->GetBuffer("depth");//得到深度缓冲区

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
		//摄像机移动
	case 'w': case 'W'://前移
		cameraPos += cameraSpeed * cameraDirection;
		cameraTarget += cameraSpeed * cameraDirection;
		break;
	case 's': case 'S'://后移
		cameraPos -= cameraSpeed * cameraDirection;
		cameraTarget -= cameraSpeed * cameraDirection;
		break;
	case 'a': case 'A'://左移
		cameraPos += cameraSpeed * cameraRight;
		cameraTarget += cameraSpeed * cameraRight;
		break;
	case 'd': case 'D'://右移
		cameraPos -= cameraSpeed * cameraRight;
		cameraTarget -= cameraSpeed * cameraRight;
		break;
	case ' ': //飞天
		cameraPos += cameraSpeed * up;
		cameraTarget += cameraSpeed * up;
		break;
	case 'x'://遁地
		cameraPos -= cameraSpeed * up;
		cameraTarget -= cameraSpeed * up;
		break;
	case 'c': case 'C'://客厅墙壁颜色切换
		//printf("切换客厅墙壁\n");
		changeWall++;
		changeWall = changeWall % 5;
		switch (changeWall) {
		case 0:
			wall_yellow = LoadObjModel("Res/livingRoomWall_Yellow.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_yellow);
			//printf("切换墙壁一\n");
			break;
		case 1:
			wall_green = LoadObjModel("Res/livingRoomWall_Green.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_green);
			//printf("切换墙壁二\n");
			break;
		case 2:
			wall_white = LoadObjModel("Res/livingRoomWall_White.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_white);
			//printf("切换墙壁三\n");
			break;
		case 3:
			wall_orange = LoadObjModel("Res/livingRoomWall_Orange.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_orange);
			//printf("切换墙壁四\n");
			break;
		case 4:
			wall_blue = LoadObjModel("Res/livingRoomWall_Blue.obj", &wallIndexes, wallVertexCount, wallIndexCount);
			wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wall_blue);
			//printf("切换墙壁五\n");
			break;
		}
		wallIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * wallIndexCount, GL_STATIC_DRAW, wallIndexes);
		//printf("changeWall=%d", changeWall);
		break;
	case 'v':case 'V'://床的切换
		//printf("切换床\n");
		changeBed++;
		changeBed = changeBed % 2;
		switch (changeBed) {
		case 0:
			bedVertexData = LoadObjModel("Res/bed.obj", &bedIndexes, bedVertexCount, bedIndexCount);
			bedVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedVertexCount, GL_STATIC_DRAW, bedVertexData);
			bedIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedIndexCount, GL_STATIC_DRAW, bedIndexes);
			//printf("床一\n");
			break;
		case 1:
			bedVertexData = LoadObjModel("Res/bed2.obj", &bedIndexes, bedVertexCount, bedIndexCount);
			bedVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedVertexCount, GL_STATIC_DRAW, bedVertexData);
			bedIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedIndexCount, GL_STATIC_DRAW, bedIndexes);
			//printf("床二\n");
			break;
		}
		//printf("changeBed=%d", changeBed);	
		break;
	case 'b':case 'B'://窗帘的切换
		//printf("切换窗帘\n");
		changeCurtain++;
		changeCurtain = changeCurtain % 2;
		switch (changeCurtain) {
		case 0:
			curtainVertexData = LoadObjModel("Res/curtain.obj", &curtainIndexes, curtainVertexCount, curtainIndexCount);
			curtainVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * curtainVertexCount, GL_STATIC_DRAW, curtainVertexData);
			curtainIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * curtainIndexCount, GL_STATIC_DRAW, curtainIndexes);
			printf("窗帘1...\n");
			break;
		case 1:
			curtainVertexData = LoadObjModel("Res/window.obj", &curtainIndexes, curtainVertexCount, curtainIndexCount);
			curtainVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * curtainVertexCount, GL_STATIC_DRAW, curtainVertexData);
			curtainIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * curtainIndexCount, GL_STATIC_DRAW, curtainIndexes);
			printf("窗帘2...\n");
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
	//滚轮控制视野的放大和缩小
	if (button == 3)
	{
		fov += 0.3f;
	}
	if (button == 4) {
		fov -= 0.3f;
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

	printf("=============键盘交互=============\n");
	printf("输入 w、s、d、a 前后左右移动...\n");
	printf("输入 空格 向上移动...\n");
	printf("输入 x 向下移动...\n");
	printf("输入 c 切换墙壁客厅...\n");
	printf("输入 v 切换床...\n");
	printf("输入 l 开关灯...\n");
	printf("输入 b 切换窗帘...\n");
	printf("输入 t 切换模型...\n");
	printf("=============鼠标交互=============\n");
	printf("拖动鼠标右键视角切换...\n");
	printf("鼠标滚轮视野放大缩小...\n");
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
	//在Simple.vs中的Pos位置向量、Normal法向量,Texcoord纹理坐标向量
	glUseProgram(renderProgram);
	glUniformMatrix4fv(renderMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));             // 向gpu program中传值
	glUniformMatrix4fv(renderVLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));			//这些参数都在53行获得位置句柄，然后在这里赋值
	projectionMatrix = glm::perspective(fov, 800.0f / 600.0f, 0.1f, 800.0f);//投影矩阵，因滚轮修改了fov参数，所以需要实时传递矩阵给着色器程序
	glUniformMatrix4fv(renderPLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(renderNMLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(renderViewPosLocation, 1, glm::value_ptr(cameraPos));
	glUniformMatrix4fv(renderLightPMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));
	glUniformMatrix4fv(renderLightVMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniform1i(renderLightOnLoc, lightOn);

	glBindTextureUnit(0, roomTexture);      // 绑定纹理，传入Light.fs的binding = 0 ，1
	glBindTextureUnit(1, shadowMap);//深度缓存存进去。用来判断是否在阴影中

	if (changeStyle == 2) {
		//绘制客厅===============================================================
		glEnableVertexAttribArray(renderPosLoc);//light.fs 位置属性
		glEnableVertexAttribArray(renderTexcoordLoc);//light.fs 纹理属性
		glEnableVertexAttribArray(renderNormalLoc);//light.fs 法向量属性

		//开始绘制客厅内部------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, roomIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//开始绘制客厅墙壁------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, wallVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, wallIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制窗帘---------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, curtainVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curtainIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, curtainIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制地板---------------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, floorIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制装饰---------------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, decoVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, decoIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, decoIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制床
		glBindBuffer(GL_ARRAY_BUFFER, bedVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bedIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, bedIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制卧室墙壁
		glBindBuffer(GL_ARRAY_BUFFER, wallVbo2);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIbo2);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, wallIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo2);
		glEnableVertexAttribArray(renderPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(renderTexcoordLoc);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(renderNormalLoc);
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo2);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, roomIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glUseProgram(0);
}

void DrawRoomSample()
{
	modelMatrix = glm::scale(2.0f, 2.0f, 2.0f);
	glUseProgram(depthProgram);
	//uniform的位置，需要加载数据的数组元素的数量或者需要修改的矩阵的数量，指明矩阵是列优先(column major)矩阵（GL_FALSE）还是行优先(row major)矩阵（GL_TRUE）指向由count个元素的数组的指针。
	glUniformMatrix4fv(depthMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(depthVLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));

	if (changeStyle == 2) {
		//开始绘制客厅内部------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, roomIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//开始绘制客厅墙壁------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, wallVbo);
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, wallIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制窗帘---------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, curtainVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curtainIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, curtainIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制地板---------------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, floorIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制装饰---------------------------------------------------------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, decoVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, decoIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, decoIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制床
		glBindBuffer(GL_ARRAY_BUFFER, bedVbo);
		glEnableVertexAttribArray(depthPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bedIbo);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, bedIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//绘制卧室墙壁
		glBindBuffer(GL_ARRAY_BUFFER, wallVbo2);
		glEnableVertexAttribArray(depthPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(depthNormalLoc);
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIbo2);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, wallIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo2);
		glEnableVertexAttribArray(renderPosLoc);                 //  利用vbo 向gpu program中传值
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(renderTexcoordLoc);
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(renderNormalLoc);
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo2);        // 利用ibo中的index绘制图形
		glDrawElements(GL_TRIANGLES, roomIndexCount2, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/**
	* 为了提升性能，节省gpu资源的耗用，可以使用背面剔除功能
	//启用背面剔除功能	
	glEnable(GL_CULL_FACE);
	//剔除的面为背面
	glCullFace(GL_BACK);
	//顶点的绘制顺序为顺时针
	glFrontFace(GL_CW);
	*/
	glUseProgram(0);
}