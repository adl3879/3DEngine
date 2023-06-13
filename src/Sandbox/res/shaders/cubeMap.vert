#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projectionViewMatrix;
uniform mat4 model;

void main()
{
    TexCoords = vec3(model * vec4(aPos, 1.0));
    vec4 pos = projectionViewMatrix * vec4(TexCoords, 1.0);
    gl_Position = pos.xyww;
};