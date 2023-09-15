#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 projectionViewMatrix;
uniform mat4 model;

void main()
{
	vec3 CurrentPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = projectionViewMatrix * vec4(CurrentPos, 1.0f);
}