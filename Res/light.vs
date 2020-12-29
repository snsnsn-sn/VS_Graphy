#version 440

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NM;
uniform mat4 LightViewMatrix;
uniform mat4 LightProjectionMatrix;

out vec3 V_Normal;
out vec4 V_WorldPos;
out vec2 V_Texcoord;
out vec4 V_LightSpaceFragPos;

void main()
{
	V_Normal=mat3(NM)*normal;
	V_WorldPos=M*vec4(pos,1.0);
	V_Texcoord=texcoord;
	V_LightSpaceFragPos = LightProjectionMatrix * LightViewMatrix * V_WorldPos;
    gl_Position=P*V*V_WorldPos;
}