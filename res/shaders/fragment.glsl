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

void main()
{
    float ambient = 0.2;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - CurrentPos.xyz);

    float diffuse = max(dot(normal, lightDirection), 0.0);

    // specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(cameraPos - CurrentPos.xyz);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

	color = texture(diffuse0, TexCoord) * lightColor * (diffuse + ambient + specular);
};
