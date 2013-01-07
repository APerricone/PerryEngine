#version 150

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D shadowTexture;

uniform vec4 lightDirPosType; 	// w:0 point 1 directional
uniform mat4 lightMatrix;
uniform mat4 invProjMatrix;
uniform vec3 camPos;

in vec2 TexCoord;
out vec4 gl_FragData;

float Shadow(vec4 posWS)
{
	vec3 posLS = (lightMatrix * posWS).xyz;
	float shadowDist = texture( shadowTexture, (posLS.xy + 1) * 0.5 ).x;

	return ( (posLS.z+1)*0.5 > shadowDist )? 0 : 1;
}

float Diffuse(vec4 posWS, vec3 normal)
{
	vec3 L = lightDirPosType.xyz;
	if( lightDirPosType.w == 0)
	{ // point
		L -= posWS.xyz;
	}
	L = normalize(L);
	return max(dot( normal.xyz, L.xyz),0.);
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

	if( Shadow(posWS) == 0) discard;

	vec3 normal = normalize((texture(normalTexture, TexCoord.xy).xyz*2.)-1.);
		
	float lInt = Diffuse(PosWS, normal);
	lInt = pow(lInt, 2.2);
	gl_FragData = vec4( lInt,lInt,lInt, 1); 
}
