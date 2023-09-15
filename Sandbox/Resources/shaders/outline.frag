#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D selectionMask;

void main()
{
	// FragColor = vec4(1.0f, 0.2f, 0.3f, 1);
    FragColor = vec4(texture(selectionMask, TexCoords).rgb, 1.0);
};
