float Shadow(vec4 posWS)
{
	vec3 posLS = (lightMatrix * posWS).xyz;
	vec3 texC = ((posLS.xyz + 1) * 0.5);
	
	if( texC.z > 1) return 1;

	float shadowDist = texture( shadowTexture, texC.xy).x;
	if( texC.z > shadowDist ) return 0;
	return 1;
}
