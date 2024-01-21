#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;
out vec3 WorldPosition;
out vec3 Normal;
out mat3 TBN;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 projectionViewMatrix;
uniform mat4 lightSpaceMatrix;

void main()
{
	vec4 totalPosition = vec4(0.0f);
	vec4 totalNormal = vec4(0.0f);
    
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (aBoneIDs[i] == -1) 
            continue;
        if (aBoneIDs[i] >= MAX_BONES) 
        {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[aBoneIDs[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];

		vec4 worldNormal = finalBonesMatrices[aBoneIDs[i]] * vec4(aNormal, 0.0f);
		totalNormal += worldNormal * aWeights[i];

        //vec3 localNormal = mat3(finalBonesMatrices[aBoneIDs[i]]) * aNormal;
    }
	
	mat4 viewModel = projectionViewMatrix * model;
    gl_Position = viewModel * totalPosition;

	vec3 N = normalize((model * vec4(aNormal, 0.0f)).xyz);
    vec3 T = normalize((model * vec4(aTangent, 0.0f)).xyz);
    vec3 B = normalize((model * vec4(aBitangent, 0.0f)).xyz);
    TBN = mat3(T, B, N);
	
    //vec3 currentPos = vec3(model * vec4(aPos, 1.0f));
    FragPosLightSpace = lightSpaceMatrix * vec4(1.0f);

    TexCoords = aUV;
    WorldPosition = aPos;
	Normal = totalNormal.xyz;
}
