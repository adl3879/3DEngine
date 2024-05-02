#version 410 core

uniform mat4 Projection;
uniform mat4 View;
uniform vec3 CameraPos;

out vec3 WorldPos;

float gridSize = 150.0;

const vec3 pos[4] = vec3[4](
	vec3(-1.0, 0.0, -1.0),
	vec3( 1.0, 0.0, -1.0),
	vec3( 1.0, 0.0,  1.0),
	vec3(-1.0, 0.0,  1.0)
);

const int indices[6] = int[6](
	0, 1, 2, 2, 3, 0
);

void main()
{
	mat4 MVP = Projection * View;

	int idx = indices[gl_VertexID];
	vec3 position = pos[idx] * gridSize;

	gl_Position = MVP * vec4(position, 1.0);

	WorldPos = position;
}
