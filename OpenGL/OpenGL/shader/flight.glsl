#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main()
{
    //float ambientStrength = 0.1;
    vec3 ambient = light.diffuse * vec3(texture(material.diffuse, TexCoord));

    vec3 normDir = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diffStrength = max(dot(normDir, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diffStrength * vec3(texture(material.diffuse, TexCoord));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normDir);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * specularStrength * vec3(texture(material.specular, TexCoord));

    vec3 result = ambient + diffuse + specular;

    vec3 emission = vec3(texture(material.emission, TexCoord));

    FragColor = vec4(result + emission, 1.0);
}