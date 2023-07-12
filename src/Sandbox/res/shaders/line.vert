#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 projectionViewMatrix;

void main()
{
    gl_Position = projectionViewMatrix * vec4(aPos, 1.0);
}