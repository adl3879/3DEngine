#version 330 core

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

layout (location = 0) out vec4 FragColor;
layout (location = 1) out int EntityId;
layout (location = 2) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 WorldPosition;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace;

struct DirectionalLight {
    vec3 Direction;
    vec3 Color;
};
uniform DirectionalLight gDirectionalLight;

struct PointLight {
    vec3 Position;
    vec3 Color;
};
uniform PointLight gPointLights[MAX_POINT_LIGHTS];

struct Spotlight {
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    float Cutoff;
    float OuterCutoff;
};
uniform Spotlight gSpotLights[MAX_SPOT_LIGHTS];

uniform vec3 cameraPosition;

// material parameters
uniform vec3 albedoParam;
uniform float metallicParam;
uniform float roughnessParam;
uniform float aoParam;
uniform float emissiveParam;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// material textures maps
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform sampler2D shadowMap;

uniform int gNumOfPointLights;
uniform int gNumOfSpotLights;
uniform int entityId;

uniform int hasAlbedoMap;
uniform int hasNormalMap;
uniform int hasMetallicMap;
uniform int hasRoughnessMap;
uniform int hasAoMap;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 calcReflectanceEquation(vec3 L, vec3 V, vec3 N, vec3 albedo, float metallic, float roughness)
{
    vec3 H = normalize(V + L);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);   
    float G = GeometrySmith(N, V, L, roughness);      
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);  
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    float NdotL = max(dot(N, L), 0.0); 
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.001;
    vec3 specular = numerator / denominator;
    
    return (kD * albedo / PI + specular) * NdotL;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

vec4 calculateFragColor(vec3 albedo, vec3 normal, float metallic, float roughness, float ao)
{
    vec3 N = normalize(normal);
    vec3 V = normalize(cameraPosition - WorldPosition);
    vec3 R = reflect(-V, N);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    float shadow = 0.0f;

    // directional light reflection
    {
        vec3 L = normalize(-gDirectionalLight.Direction);
        vec3 radiance = gDirectionalLight.Color * (1.0f - shadow);
        Lo += calcReflectanceEquation(L, V, N, albedo, metallic, roughness) * radiance;
    }

    // point lights reflection
    for (int i = 0; i < gNumOfPointLights; ++i) {
        vec3 L = normalize(gPointLights[i].Position - WorldPosition);
    
        float distance = length(gPointLights[i].Position - WorldPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = gPointLights[i].Color * attenuation; 

        Lo += calcReflectanceEquation(L, V, N, albedo, metallic, roughness) * radiance;
    }

    // spot lights reflection
    for (int i = 0; i < gNumOfSpotLights; ++i) {
        // TODO: fix spotlight
        vec3 L = normalize(gSpotLights[i].Position - WorldPosition);

        float distance = length(gSpotLights[i].Position - WorldPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = gSpotLights[i].Color * attenuation;

        float spotEffect = dot(normalize(gSpotLights[i].Direction), -L);
        if (spotEffect > gSpotLights[i].Cutoff) {
            spotEffect = pow(spotEffect, gSpotLights[i].OuterCutoff);
            Lo += calcReflectanceEquation(L, V, N, albedo, metallic, roughness) * radiance * spotEffect;
        }
    }

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    vec3 irradiance = mix(texture(irradianceMap, N).rgb, vec3(0.1f), 0.9f);
    vec3 diffuse = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    vec3 amb = (kD * (diffuse + specular)) * aoParam; // * ao;
    vec3 color = amb + Lo;

    color = color / (color + vec3(1.0));

    return vec4(color, 1.0);
}

void main() {
    // material parameters from textures
    vec3 albedo = albedoParam;
    if (hasAlbedoMap == 1) {
        albedo =  pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
        albedo = mix(albedo, albedoParam, 0.5);
    }
    // add emissive value to final albedo
    albedo *= emissiveParam;

    float metallic = metallicParam;
    if (hasMetallicMap == 1) {
        metallic = texture(metallicMap, TexCoords).r;
        metallic = mix(metallic, metallicParam, 0.5);
    }
    float roughness = roughnessParam;
    if (hasRoughnessMap == 1) {
        roughness = texture(roughnessMap, TexCoords).r;
        roughness = mix(roughness, roughnessParam, 0.5);
    }
    float ao = aoParam;
    if (hasAoMap == 1) {
        ao = texture(aoMap, TexCoords).r;
        ao = mix(ao, aoParam, 0.5);
    }
    vec3 normal = vec3(0.5, 0.5, 1.0);
    if (hasNormalMap == 1) {
        normal = texture(normalMap, TexCoords).rgb;
    }
    normal = normal * 2.0 - 1.0;
    normal = TBN * normalize(normal); // add TBN

    //custom_shader

    FragColor = calculateFragColor(albedo, normal, metallic, roughness, ao);
    
    EntityId = entityId;
}