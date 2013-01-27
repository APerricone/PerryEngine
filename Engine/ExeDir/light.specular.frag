uniform sampler2D specularTexture;

float Specular(vec4 posWS, vec3 normal, float s)
{
	vec3 L = lightDirPosType.xyz;
	if( lightDirPosType.w == 0)
	{ // point
		L -= posWS.xyz;
	}
	L = normalize(L);

	vec3 V = normalize(camPos - posWS.xyz);
	vec3 H = normalize( L + V);
	float fact = max(dot(L,normal),0.0);
	fact *= (s+8)/(8*3.1416);
	//vec3 R = reflect(-L, normal);
	return fact * pow(max(dot(H, normal), 0.0), s);
}

void main()
{ 	
	float d = texture(depthTexture, TexCoord.xy).x;
	if( d > 0.999 ) discard;
	
	vec4 posSS = vec4( (vec3(TexCoord,d)-vec3(0.5)) * vec3(2),1);

	vec4 posWS = invProjMatrix * posSS;
	posWS /= posWS.w;

	float fShadow = Shadow(posWS);
	if( fShadow == 0) discard;

	vec3 normal = normalize( (texture(normalTexture, TexCoord.xy).xyz*2.)-1. );
	vec4 s = texture(specularTexture, TexCoord.xy);
	
	float sInt = Specular(posWS, normal, s.w * 250) * fShadow;
	gl_FragData = vec4( s.xyz*sInt*lightCol, 1);
}
