float Diffuse(vec4 posWS, vec3 N)
{
	vec3 L = lightDirPosType.xyz;
	if( lightDirPosType.w == 0)
	{ // point
		L -= posWS.xyz;
	}
	L = normalize(L);
	return max(dot( N.xyz, L.xyz),0.) / 3.1416;
}

void main()
{ 	
	//if( d > 0.99 ) discard;
	if( TexCoord.x < 0 || TexCoord.x > 1 ||
		TexCoord.y < 0 || TexCoord.y > 1 ) 
	{
		gl_FragData = vec4( 1, 1, 1, 1); 
		return;
	}

	float d = texture(depthTexture, TexCoord.xy).x;
	vec4 posSS = vec4( (vec3(TexCoord,d)-vec3(0.5)) * vec3(2),1);

	vec4 posWS = invProjMatrix * posSS;
	posWS /= posWS.w;

	float aOut = Shadow(posWS);
	if( aOut == 0) discard;

	vec3 normal = normalize( (texture(normalTexture, TexCoord.xy).xyz*2.)-1. );
	float lInt = Diffuse(posWS, normal) * aOut;;
	gl_FragData = vec4( lightCol * lInt, 1);
}
