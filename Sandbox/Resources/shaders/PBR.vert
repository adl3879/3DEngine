#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aUV;
layout (location = 4) in float aEntityID;

out vec2 TexCoords;
out vec3 WorldPosition;
out vec3 Normal;
out float EntityID;

uniform mat4 model;
uniform mat4 projectionViewMatrix;
uniform mat3 normalMatrix;

void main()
{
	vec3 currentPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = projectionViewMatrix * vec4(currentPos, 1.0f);

    TexCoords = aUV;
    WorldPosition = vec3(model * vec4(aPos, 1.0f));
    Normal = normalMatrix * aNormal;
    EntityID = aEntityID;
}
