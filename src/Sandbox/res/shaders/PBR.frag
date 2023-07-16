#version 330 core

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPosition;
in vec3 Normal;

struct PointLight {
    vec3 Position;
    vec3 Color;
};
uniform PointLight gPointLights[MAX_POINT_LIGHTS];

uniform vec3 cameraPosition;

uniform vec3  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform int numOfPointLights;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness*roughness;
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

void main() {
    vec3 N = normalize(Normal);
    vec3 V = normalize(cameraPosition - WorldPosition);

    vec3 Lo = vec3(0.0);
    // point lights
    // reflectance equation
    for (int i = 0; i < numOfPointLights; ++i) {
        vec3 L = normalize(gPointLights[i].Position - WorldPosition);
        vec3 H = normalize(V + L);
    
        float distance = length(gPointLights[i].Position - WorldPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = gPointLights[1].Color * attenuation; 

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);

        // cook-torrence bdrf
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        float NDF = DistributionGGX(N, H, roughness); 
        float G = GeometrySmith(N, V, L, roughness); 

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
  
        kD *= 1.0 - metallic;	
  
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0); 
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color   = ambient + Lo;  
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}