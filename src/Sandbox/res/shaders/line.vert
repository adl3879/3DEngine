#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 projectionViewMatrix;
uniform mat4 model;

void main()
{
    gl_Position = model * projectionViewMatrix * vec4(aPos, 1.0);
}