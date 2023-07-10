#version 330 core

in vec3 Color;
in vec2 TexCoord;
in vec3 Normal;
in vec3 CurrentPos;
in float EntityID;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out int color2;

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};

struct Attenuation
{
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight
{
    BaseLight Base;
    vec3 Position;
    Attenuation Atten;
};

struct SpotLight
{
    PointLight Base;
    vec3 Direction;
    float Cutoff;
    float OuterCutoff;
};

struct Material
{
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
};

uniform Material gMaterial;
uniform DirectionalLight gDirectionalLight;
uniform int gNumPointLights;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform int gNumSpotLights;
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec3 gCameraPos;

vec4 calcLightInternal(BaseLight base, vec3 direction, vec3 normal)
{
    vec4 ambientColor = vec4(base.Color, 1.0f) *
                        base.AmbientIntensity *
                        vec4(gMaterial.AmbientColor, 1.0f);

    float diffuseFactor = dot(normal, -direction);
    vec4 diffuseColor = vec4(0, 0, 0, 0);
    vec4 specularColor = vec4(0, 0, 0, 0);
    if (diffuseFactor > 0)
    {
        diffuseColor = vec4(base.Color, 1.0f) *
                       base.DiffuseIntensity *
                       vec4(gMaterial.DiffuseColor, 1.0f) *
                       diffuseFactor;

        vec3 viewDirection = normalize(gCameraPos - CurrentPos);
        vec3 lightReflect = reflect(direction, normal);
        float specularFactor = pow(max(dot(viewDirection, lightReflect), 0.0), 32);
          
        specularColor = vec4(base.Color, 1.0f) *
                        vec4(gMaterial.SpecularColor, 1.0f) *
                        texture(specular0, TexCoord).r *
                        specularFactor;
    }

    return (ambientColor + diffuseColor + specularColor);   
}

vec4 calcDirectionalLight(vec3 normal)
{
    return calcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, normal);
}

vec4 calcPointLight(PointLight l, vec3 normal)
{
    vec3 lightDirection = CurrentPos - l.Position;
    float distance = length(lightDirection);
    lightDirection = normalize(lightDirection);

    vec4 color = calcLightInternal(l.Base, lightDirection, normal);

    float attenuation = l.Atten.Constant +
                        l.Atten.Linear * distance +
                        l.Atten.Exp * distance * distance;

    return color / attenuation;
}

vec4 calcSpotLight(SpotLight l, vec3 normal)
{
    vec3 lightDirection = normalize(CurrentPos - l.Base.Position);
    float spotFactor = dot(lightDirection, normalize(-l.Direction));
    float epsilon = l.Cutoff - l.OuterCutoff;

    if (spotFactor > l.Cutoff)
    {
        vec4 color = calcPointLight(l.Base, normal);
        float spotLightIntensity = clamp((spotFactor - l.OuterCutoff) / epsilon, 0.0, 1.0);
        return color * spotLightIntensity;
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}

void main()
{
    vec3 normal = normalize(Normal);
    vec4 totalLight = calcDirectionalLight(normal);

    for (int i = 0; i < gNumPointLights; i++)
    {
        totalLight += calcPointLight(gPointLights[i], normal);
    }

    for (int i = 0; i < gNumSpotLights; i++)
    {
        totalLight += calcSpotLight(gSpotLights[i], normal);
    }

    FragColor = texture(diffuse0, TexCoord) * totalLight;
    color2 = int(EntityID);
}