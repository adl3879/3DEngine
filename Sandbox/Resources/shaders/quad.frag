#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D outlineTexture;
uniform float exposure;
uniform float bloomStrength = 0.04f;
uniform int bloomEnabled;

vec3 bloom_none()
{
  vec3 hdrColor = texture(scene, TexCoords).rgb;
  return hdrColor;
}

vec3 bloom()
{
  vec3 hdrColor = texture(scene, TexCoords).rgb;
  vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
  //return mix(hdrColor, bloomColor, bloomStrength); // linear interpolation
  
  return hdrColor + bloomColor * bloomStrength; // additive blending
}

void main()
{
	// to bloom or not to bloom
	vec3 result = vec3(0.0);
	if (bloomEnabled == 0)
		result = bloom_none();
	else
		result = bloom();

	// tone mapping
	result = vec3(1.0) - exp(-result * exposure);
	// also gamma correct while we're at it
	const float gamma = 2.2;
	result = pow(result, vec3(1.0 / gamma));
  
	vec4 outlineColor = texture(outlineTexture, TexCoords);

	//FragColor = mix(vec4(result, 1.0), outlineColor, outlineColor.a);
	FragColor = texture(scene, TexCoords);

//	float depthValue = texture(scene, TexCoords).r;
//	FragColor = vec4(vec3(depthValue), 1.0);
}
