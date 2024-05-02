#version 440 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 UVPosition;

out flat vec2 a_UV;

void main()
{
	a_UV = UVPosition;
	gl_Position = vec4(VertexPosition, 1.0f);
}
