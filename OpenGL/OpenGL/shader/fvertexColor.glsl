#version 330 core
out vec4 FragColor;
in vec3 myColor2;
void main()
{
	FragColor = vec4(myColor2, 1.0f);
}