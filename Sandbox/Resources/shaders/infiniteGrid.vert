#version 330 core

layout (location = 0) in vec3 aPos;

layout(location = 0) out float near;
layout(location = 1) out float far;
layout(location = 2) out vec3 nearPoint;
layout(location = 3) out vec3 farPoint;
layout(location = 4) out mat4 fragView;
layout(location = 8) out mat4 fragProj;

uniform mat4 Projection;
uniform mat4 View;

// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[](
  vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
  vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
  mat4 viewInv = inverse(view);
  mat4 projInv = inverse(projection);
  vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
  return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
  vec3 p = gridPlane[gl_VertexID].xyz;

  fragProj = Projection;
  fragView = View;

  near = gl_DepthRange.near;
  far = gl_DepthRange.far;

  nearPoint = UnprojectPoint(p.x, p.y, 0.0, View, Projection).xyz;
  farPoint = UnprojectPoint(p.x, p.y, 1.0, View, Projection).xyz;

  gl_Position = vec4(p, 1.0);
}