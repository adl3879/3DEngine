#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 WorldPosition;
out vec3 Normal;
out mat3 TBN;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 projectionViewMatrix;
uniform mat4 lightSpaceMatrix;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal = normalMatrix * aNormal;

    vec3 N = normalize((model * vec4(aNormal, 0.0f)).xyz);
    vec3 T = normalize((model * vec4(aTangent, 0.0f)).xyz);
    vec3 B = normalize((model * vec4(aBitangent, 0.0f)).xyz);
    TBN = mat3(T, B, N);
	
    vec3 currentPos = vec3(model * vec4(aPos, 1.0f));

    FragPosLightSpace = lightSpaceMatrix * vec4(currentPos, 1.0f);
    TexCoords = aUV;
    WorldPosition = currentPos;

    gl_Position = projectionViewMatrix * vec4(currentPos, 1.0f);
}
