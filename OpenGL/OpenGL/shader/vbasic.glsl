#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 myColor2;

uniform float offset;

void main()
{
	gl_Position = vec4(aPos.x + offset, aPos.y +offset, aPos.z, 1.0);
	myColor2 = aColor;
}