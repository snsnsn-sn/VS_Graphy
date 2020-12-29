#pragma once

/*
	存储顶点信息的结构体
	float position   顶点位置信息
	float texcoord   顶点纹理坐标
	float normal     顶点法线信息
*/
struct VertexData        
{
	float position[3];
	float texcoord[2];
	float normal[3];
};

/*
	读取obj文件，获取obj顶点信息
	@param  filePath   obj文件名
	@param  indexes    顶点的索引信息
	@param  vertexCount   顶点个数
	@param  indexCount    索引个数
	@retval 存储obj信息的VertexData
*/
VertexData*LoadObjModel(const char* filePath,unsigned int **indexes,int&vertexCount,int&indexCount);