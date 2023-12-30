#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec3 currentPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = (projection * view) * vec4(currentPos, 1.0f);
}