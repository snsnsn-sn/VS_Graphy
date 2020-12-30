#include"misc.h"
#include"FBO.h"
#include"Param.h"
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
	//VBO就是把要绘制的顶点信息直接缓存在显卡内存中,VBO为顶点缓冲区对象，用于存储顶点坐标/顶点uv/顶点法线/顶点颜色，
	roomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount,
		GL_STATIC_DRAW, roomVertexData);//创建vbo缓存，第一个参数是数据类型，第二个是顶点个数，第三个是，第四个是顶点信息
	//IBO为索引缓冲区，里面的值可以是unsigned int或者unsigned short。
	roomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount,	GL_STATIC_DRAW, roomIndexes);	   // 创建vbo  ibo
	//第二个房间模型
	roomVertexData2 = LoadObjModel("Res/room2.obj", &roomIndexes2, roomVertexCount2, roomIndexCount2);

	roomVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount2,
		GL_STATIC_DRAW, roomVertexData2);
	roomIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount2, GL_STATIC_DRAW, roomIndexes2);

	glEnable(GL_DEPTH_TEST);

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
	
	/*case 'm': case 'M':
		changecolor += 0.5;
		if (changecolor > 2) changecolor = 0;
		break;*/
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
	//在Simple.vs中的Pos位置向量、Normal法向量,Texcoord纹理坐标向量
	glUseProgram(renderProgram);
	glUniformMatrix4fv(renderMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));             // 向gpu program中传值
	glUniformMatrix4fv(renderVLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));			//这些参数都在47行赋值
	glUniformMatrix4fv(renderPLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(renderNMLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(renderViewPosLocation, 1, glm::value_ptr(cameraPos));
	glUniformMatrix4fv(renderLightPMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));
	glUniformMatrix4fv(renderLightVMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniform1i(renderLightOnLoc, lightOn);

	glBindTextureUnit(0, roomTexture);      // 绑定纹理，传入Light.fs的binding = 0 ，1
	glBindTextureUnit(1, shadowMap);//深度缓存存进去。用来判断是否在阴影中

	if (bDrawRoom1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);              
		glEnableVertexAttribArray(renderPosLoc);                 //  利用vbo 向gpu program中传值
		//顶点位置属性
		glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(renderTexcoordLoc);
		//顶点纹理属性
		glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(renderNormalLoc);
		//顶点颜色属性
		glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomIbo);        // 利用ibo中的index绘制图形
		/**   void glDrawElements(GLenum mode,GLsizei count,GLenum type,const GLvoid *indices);
		*mode:接受的值和在glBegin()中接受的值一样，可以是GL_POLYGON、GL_TRIANGLES、GL_TRIANGLE_STRIP、GL_LINE_STRIP等。
		*count：组合几何图形的元素的个数，一般是点的个数。
		*type:indeices数组的数据类型，既然是索引，一般是整型的。
		*indices:索引数组
		*/
		glDrawElements(GL_TRIANGLES, roomIndexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//将当前活动内存设置为空，非必须
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
	//uniform的位置，需要加载数据的数组元素的数量或者需要修改的矩阵的数量，指明矩阵是列优先(column major)矩阵（GL_FALSE）还是行优先(row major)矩阵（GL_TRUE）指向由count个元素的数组的指针。
	glUniformMatrix4fv(depthMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(depthVLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));

	if (bDrawRoom1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);
		
		glEnableVertexAttribArray(depthPosLoc);//glEnableVertexAttribArray(index)启用index指定的通用顶点属性数组
		glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);//第二个参数指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3。
		//传到vs的layout=0,1,2的位置
		glEnableVertexAttribArray(depthTexcoordLoc);
		glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
		
		glEnableVertexAttribArray(depthNormalLoc);//第四个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
		glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));
		//第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。由于下个组位置数据在1个float之后，我们把步长设置为1 * sizeof(float)

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