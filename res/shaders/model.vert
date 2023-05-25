#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 Color;
out vec2 TexCoord;
out vec3 Normal;
out vec4 CurrentPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	CurrentPos = projection * view * model * translation * -rotation * scale * vec4(aPos, 1.0f);
    gl_Position = CurrentPos;

	Color = aColor;
	TexCoord = aTex;
	Normal = aNormal;
}
