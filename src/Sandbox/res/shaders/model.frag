#version 330 core

in vec3 Color;
in vec2 TexCoord;
in vec3 Normal;
in vec4 CurrentPos;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 color2;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform int lightType;

struct DirectionalLight
{
    vec3 direction;
    float ambient;
    float specularLight;
};
uniform DirectionalLight dLight;

struct PointLight
{
    float constant;
    float linear;
    float quadratic;
    float ambient;
    float specularLight;
};
uniform PointLight pLight;

struct SpotLight
{
    float ambient;
    float specularLight;
    float cutOff;
    float outerCutOff;
};
uniform SpotLight sLight;

vec4 pointLight()
{
    vec3 lightVec = lightPos - CurrentPos.xyz;
    // intensity of light with respect to distance
    float dist = length(lightVec);
    float a = pLight.quadratic;
    float b = pLight.linear;
    float c = pLight.constant;
    float inten = 1.0f / (a * dist * dist + b * dist + c);

    float ambient = pLight.ambient;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVec);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
	float specularLight = pLight.specularLight;
	vec3 viewDirection = normalize(cameraPos - CurrentPos.xyz);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

    return (texture(diffuse0, TexCoord) * (diffuse * inten + ambient) + texture(specular0, TexCoord).r * specular * inten) * lightColor;
}

vec4 directionalLight()
{
    // ambient lighting
    float ambient = dLight.ambient;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(dLight.direction);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(cameraPos - CurrentPos.xyz);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * dLight.specularLight;

    return (texture(diffuse0, TexCoord) * (diffuse + ambient) + texture(specular0, TexCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
    float outerCone = sLight.outerCutOff;
    float innerCone = sLight.cutOff;

    float ambient = sLight.ambient;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - CurrentPos.xyz);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
	float specularLight = sLight.specularLight;
	vec3 viewDirection = normalize(cameraPos - CurrentPos.xyz);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

    float angle = dot(lightPos - CurrentPos.xyz, -lightDirection);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

    return (texture(diffuse0, TexCoord) * (diffuse * inten + ambient) + texture(specular0, TexCoord).r * specular * inten) * lightColor;
}

void main()
{
    // if (lightType == 0)
    //     color = pointLight();
    // else if (lightType == 1)
    //     color = spotLight();
    // else if (lightType == 2)
    //     color = directionalLight();
    // else
    //     color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    color2 = vec4(1.0f, 0.2f, 0.3f, 0.0f);
};
