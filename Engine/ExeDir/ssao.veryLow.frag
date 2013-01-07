#version 150

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform float intensity;
uniform vec3 zValues;

in vec2 TexCoord;
out vec4 gl_FragData;

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
	return GetZ(LinearizeZ( texture(depthTexture, TexCoord.xy).x ));
}

void main()
{ 	
	float zPos = texture(depthTexture, TexCoord.xy).x;
	if( zPos > 0.99 ) discard;

	//float radius = 1 * zPos * 

	float z = GetZ(LinearizeZ( texture(depthTexture, TexCoord.xy).x ));
	float a = 1;
	float zc,d;

	zc = GetZ(LinearizeZ( textureOffset(depthTexture, TexCoord.xy, ivec2( 1,0)).x ));
	d = abs(z-zc); 
	if( d < 0.1 ) a -= d*10/4;
	//if( d > 0.1 ) d = 0; else d*=10; a += d;
	zc = GetZ(LinearizeZ( textureOffset(depthTexture, TexCoord.xy, ivec2(-1,0)).x ));
	d = abs(z-zc); 
	if( d < 0.1 ) a -= d*10/4;
	//if( d > 0.1 ) d = 0; else d*=10; a += d;
	zc = GetZ(LinearizeZ( textureOffset(depthTexture, TexCoord.xy, ivec2(0, 1)).x ));
	d = abs(z-zc); 
	if( d < 0.1 ) a -= d*10/4;
	//if( d > 0.1 ) d = 0; else d*=10; a += d;
	zc = GetZ(LinearizeZ( textureOffset(depthTexture, TexCoord.xy, ivec2(0,-1)).x ));
	d = abs(z-zc); 
	if( d < 0.1 ) a -= d*10/4;
	//if( d > 0.1 ) d = 0; else d*=10; a += d;


	gl_FragData = vec4(1,1,1,0) * a * intensity;
	//texture(image, TexCoord.xy);
}
