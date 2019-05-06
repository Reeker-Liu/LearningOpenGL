#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Color;

struct Material{
    float shininess;
};

struct DirLight{
    vec3 direction;

    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;

    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

//should extend PointLight
struct SpotLight{
    vec3 position;
    vec3 direction;

    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutoff;
    float outerCutoff;
};

uniform Material material;

uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_emission1;

uniform bool useTexture;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos);

void main()
{
    vec3 normDir = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calcDirLight(dirLight, normDir, viewDir);
    result += calcPointLight(pointLight, normDir, viewDir, FragPos);
    result += calcSpotLight(spotLight, normDir, viewDir, FragPos);

    vec3 emission;
    if (useTexture)
        emission = vec3(texture(texture_emission1, TexCoord));
    else
        emission = vec3(0.0);

    FragColor = vec4(result + emission, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    if (useTexture)
    {
        diffuse *= vec3(texture(texture_diffuse1, TexCoord));
        specular *= vec3(texture(texture_specular1, TexCoord));
    }
    else
    {
        diffuse *= Color;
        specular *= Color;
    }

    return (diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float dis = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * dis * dis);

    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    if (useTexture)
    {
        diffuse *= vec3(texture(texture_diffuse1, TexCoord));
        specular *= vec3(texture(texture_specular1, TexCoord));
    }
    else
    {
        diffuse *= Color;
        specular *= Color;
    }
    diffuse *= attenuation;
    specular *= attenuation;

    return (diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = spotLight.cutoff - spotLight.outerCutoff;
    float intensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0, 1.0);
    if(intensity < 0.001)
        return vec3(0.0);
    
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float dis = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * dis * dis);

    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    if (useTexture)
    {
        diffuse *= vec3(texture(texture_diffuse1, TexCoord));
        specular *= vec3(texture(texture_specular1, TexCoord));
    }
    else
    {
        diffuse *= Color;
        specular *= Color;
    }
    diffuse *= attenuation;
    specular *= attenuation;

    return intensity * (diffuse + specular);
    
}