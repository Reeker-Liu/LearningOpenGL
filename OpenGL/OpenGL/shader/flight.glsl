#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 lightPos;

void main()
{
    vec3 normDir = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diffStrength = max(dot(normDir, lightDir), 0.0);
    float ambientStrength = 0.1;

    float strength = diffStrength + ambientStrength;

    FragColor = vec4(strength * lightColor * objectColor, 1.0);
}