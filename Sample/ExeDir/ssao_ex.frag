#version 150

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform float intensity;
uniform vec2 zValues;

in vec2 TexCoord;
out vec4 gl_FragData;

#define N_SAMPLE 8
//float c[3] = float[3](5.0, b + 1.0, 1.1);
vec2 samples[N_SAMPLE] = vec2[N_SAMPLE]( 
	vec2( 1, 0), vec2( 0.707, 0.707),
	vec2( 0, 1), vec2(-0.707, 0.707),
	vec2(-1, 0), vec2(-0.707,-0.707),
	vec2( 0,-1), vec2( 0.707,-0.707) );

float LinearizeZ(float z)
{
  float n = zValues.x;
  float f = zValues.y;
  return (2.0 * n) / (f + n - z * (f - n));
}

float GetZ(float z)
{
  float n = zValues.x;
  float f = zValues.y;
  return n + (z * (f - n));
}

float GetDepth(vec2 p)
{
	return /*GetZ*/( LinearizeZ( texture(depthTexture, TexCoord.xy).x ));
}


void main()
{ 	
/*
	vec2 d = vec2(0,0.01);
	ivec2 texSize = textureSize(depthTexture,0);
	d.x = d.y * texSize.x / texSize.y;*/

	float z = GetDepth(TexCoord.xy);
	/*
	vec3 normal = normalize( (texture(normalTexture, TexCoord.xy).xyz*2.)-1. );
	vec3 up = vec3(0,1,0);
	vec3 rg = normalize(cross(up,normal));
	up = normalize(cross(rg,normal));
	*/
	float a = 0;
	for(int i=0;i<N_SAMPLE;i++)
	{
		vec2 t = TexCoord + samples[i] * d;
		float zc = GetDepth(t);
		if( zc>z-0.1 && zc<z+0.1 )
			a += abs(z-zc);
	}

	gl_FragData = vec4(a,a,a,0); //vec4(1,1,1,0) * a * intensity;
	//texture(image, TexCoord.xy);
}
