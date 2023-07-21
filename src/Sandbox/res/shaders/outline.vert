#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projectionViewMatrix;

void main()
{
	vec3 CurrentPos = vec3(model * vec4(aPos + aNormal * 0.05, 1.0f));
    Normal = aNormal;
    TexCoords = aTexCoord;
    gl_Position = projectionViewMatrix * vec4(CurrentPos, 1.0f);
}
