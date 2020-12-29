#pragma once

/*
	�洢������Ϣ�Ľṹ��
	float position   ����λ����Ϣ
	float texcoord   ������������
	float normal     ���㷨����Ϣ
*/
struct VertexData        
{
	float position[3];
	float texcoord[2];
	float normal[3];
};

/*
	��ȡobj�ļ�����ȡobj������Ϣ
	@param  filePath   obj�ļ���
	@param  indexes    �����������Ϣ
	@param  vertexCount   �������
	@param  indexCount    ��������
	@retval �洢obj��Ϣ��VertexData
*/
VertexData*LoadObjModel(const char* filePath,unsigned int **indexes,int&vertexCount,int&indexCount);