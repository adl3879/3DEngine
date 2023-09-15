#version 460 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gMaterial;
layout(location = 3) out int gEntityID;

in vec3 FragPos;
in vec2 UV;
in mat3 TBN;

// Material
uniform sampler2D m_Albedo;
uniform sampler2D m_Metalness;
uniform sampler2D m_Roughness;
uniform sampler2D m_AO;
uniform sampler2D m_Normal;
uniform sampler2D m_Displacement;
uniform int u_EntityID;

layout(std140, binding = 32) uniform u_MaterialUniform
{
    int u_HasAlbedo;
    vec3 m_AlbedoColor;     //  16 byte
    int u_HasMetalness;     //  32 byte
    float u_MetalnessValue; //  36 byte
    int u_HasRoughness;     //  40 byte
    float u_RoughnessValue; //  44 byte
    int u_HasAO;            //  48 byte
    float u_AOValue;        //  52 byte
    int u_HasNormal;        //  56 byte
    int u_HasDisplacement;  //  60 byte
    int u_Unlit;
};

void main()
{
    // Normal
    vec3 normal = vec3(0.5, 0.5, 1.0);
    if (u_HasNormal == 1)
        normal = texture(m_Normal, UV).rgb;
    normal = normal * 2.0 - 1.0;
    normal = TBN * normalize(normal);
    gNormal = vec4(normal / 2.0 + 0.5, 1.0);

    // Albedo
    gAlbedo = vec4(m_AlbedoColor, 1.0);
    if (u_HasAlbedo == 1)
    {
        vec4 albedoSample = texture(m_Albedo, UV);
        gAlbedo.rgb = albedoSample.rgb * m_AlbedoColor.rgb;
        gAlbedo.a = albedoSample.a;

        if (albedoSample.a < 0.1f)
        {
            discard;
        }
    }

    // Material
    float finalMetalness = u_MetalnessValue;
    if (u_HasMetalness == 1)
        finalMetalness = texture(m_Metalness, UV).r;

    float finalAO = u_AOValue;
    if (u_HasAO == 1)
        finalAO = texture(m_AO, UV).r;

    float finalRoughness = u_RoughnessValue;
    if (u_HasRoughness == 1)
        finalRoughness = texture(m_Roughness, UV).r;

    gMaterial = vec4(0, 0, 0, 1);
    gMaterial.r = finalMetalness;
    gMaterial.g = finalAO;
    gMaterial.b = finalRoughness;

    gEntityID = int(u_EntityID);
}