#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 projectionViewMatrix;
uniform float outlining;

void main()
{
	vec3 CurrentPos = vec3(model * vec4(aPos + aNormal * outlining, 1.0f));
    gl_Position = projectionViewMatrix * vec4(CurrentPos, 1.0f);
}
