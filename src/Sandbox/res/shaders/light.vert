#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 projectionViewMatrix;

void main()
{
	vec3 CurrentPos = vec3(vec4(aPos, 1.0f));
    gl_Position = projectionViewMatrix * vec4(CurrentPos, 1.0f);
}