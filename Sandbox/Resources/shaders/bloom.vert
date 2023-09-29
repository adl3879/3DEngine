#version 460 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 UVPosition;

out flat vec2 UV;

void main()
{
  UV = UVPosition;
  gl_Position = vec4(VertexPosition, 1.0f);
}