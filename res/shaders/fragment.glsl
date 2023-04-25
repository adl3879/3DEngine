#version 330 core

in vec3 Color;
in vec2 TexCoord;

out vec4 color;
uniform sampler2D diffuse0;

void main()
{
	color = texture(diffuse0, TexCoord);
};
