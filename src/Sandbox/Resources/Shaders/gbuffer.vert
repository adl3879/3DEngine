#version 460 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 UVPosition;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 Bitangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;
out mat3 TBN;
out vec2 UV;

void main()
{
    //mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    //vec3 T = normalize(normalMatrix * Tangent);
    //vec3 N = normalize(normalMatrix * Normal);
    //T = normalize(T - dot(T, N) * N);
    //vec3 B = cross(N, T);

    vec3 T = normalize((u_Model * vec4(Tangent, 0.0f)).xyz);
    vec3 N = normalize((u_Model * vec4(Normal, 0.0f)).xyz);
    vec3 B = normalize((u_Model * vec4(Bitangent, 0.0f)).xyz);
    TBN = mat3(T, B, N);

    UV = UVPosition;

    gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0f);
}
