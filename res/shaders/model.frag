#version 330 core

in vec3 Color;
in vec2 TexCoord;
in vec3 Normal;
in vec4 CurrentPos;

out vec4 color;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

vec4 pointLight()
{
    vec3 lightVec = lightPos - CurrentPos.xyz;
    // intensity of light with respect to distance
    float dist = length(lightVec);
    float a = 0.05;
    float b = 0.01;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

    float ambient = 0.2;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVec);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(cameraPos - CurrentPos.xyz);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

    return (texture(diffuse0, TexCoord) * (diffuse * inten + ambient) + texture(specular0, TexCoord).r * specular * inten) * lightColor;
}

vec4 directionalLight()
{
    // ambient lighting
    float ambient = 0.2;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(cameraPos - CurrentPos.xyz);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return (texture(diffuse0, TexCoord) * (diffuse + ambient) + texture(specular0, TexCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
    float outerCone = 0.90f;
    float innerCone = 0.95f;

    float ambient = 0.20f;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - CurrentPos.xyz);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(cameraPos - CurrentPos.xyz);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

    return (texture(diffuse0, TexCoord) * (diffuse * inten + ambient) + texture(specular0, TexCoord).r * specular * inten) * lightColor;
}

void main()
{
	color = directionalLight();
};
