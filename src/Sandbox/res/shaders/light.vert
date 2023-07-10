#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 Color;
out vec2 TexCoord;
out vec3 Normal;
out vec3 CurrentPos;

uniform mat4 model;
uniform mat4 projectionViewMatrix;

void main()
{
	CurrentPos = vec3(vec4(aPos, 1.0f));
    gl_Position = projectionViewMatrix * vec4(CurrentPos, 1.0f);

	Color = aColor;
	TexCoord = aTex;
	Normal = aNormal;
}