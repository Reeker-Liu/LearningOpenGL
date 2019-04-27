#version 330 core

out vec4 FragColor;

in vec3 myColor;
in vec2 texCoor;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixValue;

void main()
{
    FragColor = mix(texture(texture1, texCoor), texture(texture2,texCoor), mixValue);
}