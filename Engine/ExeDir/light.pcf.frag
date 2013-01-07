float Shadow(vec4 posWS)
{
	vec3 posLS = (lightMatrix * posWS).xyz;
	ivec2 texSize = textureSize(shadowTexture,0);

	vec3 texC = ((posLS.xyz + 1) * 0.5);
	vec2 facts = fract(texC.xy * texSize);
	vec2 ifacts = vec2(1,1)-facts;
	
	if( texC.z > 1) return 1;

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
