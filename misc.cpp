#include "misc.h"
#include <stdio.h>
#include"SOIL2.h"
#include<stdlib.h>
#include<gl/freeglut.h>
void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog); 
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data /* = nullptr */)
{
	GLuint object;
	glGenBuffers(1, &object);//创建一个帧缓冲对象(Framebuffer Object, FBO)：
	glBindBuffer(bufferType, object);//第一个参数指定绑定的参数类型，
	glBufferData(bufferType, size, data, usage);//把用户定义的数据传输到当前绑定的显存缓冲区中
	glBindBuffer(bufferType, 0);//usage指定数据存储的预期使用模式。 符号常量必须为GL_STREAM_DRAW，GL_STATIC_DRAW或GL_DYNAMIC_DRAW
	return object;
}

char *LoadFileContent(const char*path)
{
	FILE*pFile = fopen(path, "rb");
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		int nLen = ftell(pFile);
		char*buffer = nullptr;
		if (nLen!=0)
		{
			buffer=new char[nLen + 1];
			rewind(pFile);
			fread(buffer, nLen, 1, pFile);
			buffer[nLen] = '\0';
		}
		else
		{
			printf("load file fail %s content len is 0\n", path);
		}
		fclose(pFile);
		return buffer;
	}
	else
	{
		printf("open file %s fail\n",path);
	}
	fclose(pFile);
	return nullptr;
}

GLuint CreateTexture(const char* imgFileName)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	return SOIL_load_OGL_texture(imgFileName, 0, 0, SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_INVERT_Y);
}

GLuint CreateGPUProgram(const char* vsShaderPath, const char* fsShaderPath)
{//首先创建一个对象作为shader的容器，这个创建函数将返回容器的句柄。
	GLuint vsShader = glCreateShader(GL_VERTEX_SHADER);  // GL_VERTEX_SHADER类型的着色器是一个用于在可编程顶点处理器上运行的着色器
	GLuint fsShader = glCreateShader(GL_FRAGMENT_SHADER);	//GL_FRAGMENT_SHADER类型的着色器是一个着色器，旨在在可编程片段处理器上运行
	const char* vsCode = LoadFileContent(vsShaderPath);
	const char* fsCode = LoadFileContent(fsShaderPath);
	glShaderSource(vsShader, 1, &vsCode, nullptr);//添加源代码。shader的源代码是一个字符串数组
	glShaderSource(fsShader, 1, &fsCode, nullptr);//ram -> vram
	glCompileShader(vsShader);//编译shader
	glCompileShader(fsShader);
	printShaderInfoLog(vsShader);
	printShaderInfoLog(fsShader);
	GLuint program = glCreateProgram();//首先创建一个对象，作为程序的容器
	glAttachShader(program, vsShader);//把上一节编译的shader附加到刚刚创建的程序中
	glAttachShader(program, fsShader);
	glLinkProgram(program);//连接程序
	glDetachShader(program, vsShader);//附加一个shader到一个程序中，这里的调用是将shader从程序中分离
	glDetachShader(program, fsShader);
	glDeleteShader(vsShader);//如果一个shader还附加在某个程序中，这个shader并不能真正删除，只能标记为删除。当这个shader从所有程序中分离之后，才会被最终删除。
	glDeleteShader(fsShader);
	return program;
}
