#version 440 core

uniform int entityID;
uniform usampler2D entityTexture;
uniform vec4 outlineColor;

out vec4 FragColor;

in vec2 a_UV;

void main()
{
    int target = entityID;
    const float TAU = 6.28318530;
	const float steps = 32.0;
    
	float radius = 4.f;
	vec2 uv = a_UV;
    
	// sample middle
	uint middleSample = texture(entityTexture, uv).r;

    // Correct aspect ratio
    vec2 aspect = 1.0 / vec2(textureSize(entityTexture, 0));
    
	float hasHit = 0.0f;
	vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0f);
	for (float i = 0.0; i < TAU; i += TAU / steps) 
    {
		// Sample image in a circular pattern
        vec2 offset = vec2(sin(i), cos(i)) * aspect * radius;
		uint col = texture(entityTexture, uv + offset).r;
		
		if(col == target)
		{
			hasHit = 1.0f;
		}

		// Mix outline with background
		float alpha = smoothstep(0.5, 0.9, int(col != target) * hasHit * 10.0f);
		fragColor = mix(fragColor, outlineColor, alpha);
	}
	
    if(fragColor.a > 0.1)
    {
        fragColor.a = 1.0f;
    }
    
    FragColor = mix(vec4(0), fragColor, middleSample != target && hasHit > 0.0f);
}
