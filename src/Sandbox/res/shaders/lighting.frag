#version 330 core

in vec3 Color;
in vec2 TexCoord;
in vec3 Normal;
in vec4 CurrentPos;
in vec3 LocalPos;

out vec4 FragColor;

struct DirectionalLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3 Direction;
};

struct Material
{
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
};

uniform Material gMaterial;
uniform DirectionalLight gDirectionalLight;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec3 gCameraPos;

void main()
{
    vec3 normal = normalize(Normal);

    vec4 ambientColor = vec4(gDirectionalLight.Color, 1.0f) *
                        gDirectionalLight.AmbientIntensity *
                        vec4(gMaterial.AmbientColor, 1.0f);

    float diffuseFactor = dot(normal, -gDirectionalLight.Direction);
    vec4 diffuseColor = vec4(0, 0, 0, 0);
    vec4 specularColor = vec4(0, 0, 0, 0);
    if (diffuseFactor > 0)
    {
        diffuseColor = vec4(gDirectionalLight.Color, 1.0f) *
                       gDirectionalLight.DiffuseIntensity *
                       vec4(gMaterial.DiffuseColor, 1.0f) *
                       diffuseFactor;

        vec3 viewDirection = normalize(gCameraPos - CurrentPos.xyz);
        vec3 lightReflect = reflect(-gDirectionalLight.Direction, normal);
        float specularFactor = pow(max(dot(viewDirection, lightReflect), 0.0), 32);
          
        specularColor = vec4(gDirectionalLight.Color, 1.0f) *
                        vec4(gMaterial.SpecularColor, 1.0f) *
                        // texture(specular0, TexCoord).r *
                        specularFactor;
    }

    FragColor = texture(diffuse0, TexCoord) * (ambientColor + diffuseColor + specularColor);
};