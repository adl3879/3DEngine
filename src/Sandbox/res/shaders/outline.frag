#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D binaryMask;

void main()
{
	FragColor = vec4(1.0f, 0.2f, 0.3f, 1);
    // FragColor = vec4(texture(binaryMask, TexCoords).rgb, 1.0);
};
