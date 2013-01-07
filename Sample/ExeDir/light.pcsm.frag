#version 150

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D shadowTexture;

uniform vec4 lightDirPosType; 	// w:0 point 1 directional
uniform vec3 lightCol; 	// w:0 point 1 directional
uniform mat4 lightMatrix;
uniform mat4 invProjMatrix;
uniform vec3 camPos;

in vec2 TexCoord;
out vec4 gl_FragData;

float Shadow(vec4 posWS)
{
	vec3 posLS = (lightMatrix * posWS).xyz;
	ivec2 texSize = textureSize(shadowTexture,0);

	vec3 texC = ((posLS.xyz + 1) * 0.5);
	vec2 facts = fract(texC.xy * texSize);
	vec2 ifacts = vec2(1,1)-facts;
	
	float aOut = 1;
	float shadowDist;

	shadowDist = texture( shadowTexture, texC.xy).x;
	if( texC.z > shadowDist ) aOut -= ifacts.x * ifacts.y;
	shadowDist = textureOffset( shadowTexture, texC.xy, ivec2(1,0)).x;
	if( texC.z > shadowDist ) aOut -= facts.x * ifacts.y;
	shadowDist = textureOffset( shadowTexture, texC.xy, ivec2(0,1)).x;
	if( texC.z > shadowDist ) aOut -= ifacts.x * facts.y;
	shadowDist = textureOffset( shadowTexture, texC.xy, ivec2(1,1)).x;
	if( texC.z > shadowDist ) aOut -= facts.x * facts.y;

	return aOut;
}

float Diffuse(vec4 posWS, vec3 N)
{
	vec3 L = lightDirPosType.xyz;
	if( lightDirPosType.w == 0)
	{ // point
		L -= posWS.xyz;
	}
	L = normalize(L);
	return max(dot( N.xyz, L.xyz),0.);
}

void main()
{ 	
	float d = texture(depthTexture, TexCoord.xy).x;
	if( d > 0.99 ) discard;
	
	vec4 posSS = vec4( 
		(TexCoord.x-0.5)*2, 
		(TexCoord.y-0.5)*2, 
		(d-0.5)*2,1);

	vec4 posWS = invProjMatrix * posSS;
	posWS /= posWS.w;

	float aOut = Shadow(posWS);
	if( aOut == 0) discard;

	vec3 normal = normalize( (texture(normalTexture, TexCoord.xy).xyz*2.)-1. );
	float lInt = Diffuse(posWS, normal) * aOut;;
	gl_FragData = vec4( lInt,lInt,lInt, 1); 
}
