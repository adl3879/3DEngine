#version 460 core

layout (location=0) out vec2 uv;
layout (location=1) out vec2 out_camPos;

uniform mat4 Projection;
uniform mat4 View;
uniform vec3 CameraPos;

float gridSize = 100.0;

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
	
	position.x += CameraPos.x;
	position.z += CameraPos.z;

	out_camPos = CameraPos.xz;

	gl_Position = MVP * vec4(position, 1.0);
	uv = position.xz;
}
