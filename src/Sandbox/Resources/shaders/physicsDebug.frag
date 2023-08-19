#version 330 core
in vec3 Color;
in vec2 TexCoord;
in vec3 Normal;
in vec3 CurrentPos;
in float EntityID;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out int color2;

void main()
{
    FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    color2 = int(EntityID);
}