#version 150

uniform sampler2D image;
uniform sampler3D lut;

in vec2 TexCoord;
out vec4 gl_FragData;

float getLum(vec3 c)
{
	return dot(c, vec3(0.299, 0.587, 0.114) );
}
/*
vec3 nylon()
{
	vec3 color1 = texture(image, TexCoord.xy).xyz;
	vec3 color2 = color1 * (1/2.0);
	color2 += textureLod(image, TexCoord.xy,1).xyz * (1/4.0);
	color2 += textureLod(image, TexCoord.xy,2).xyz * (1/8.0);
	color2 += textureLod(image, TexCoord.xy,3).xyz * (1/16.0);
	color2 += textureLod(image, TexCoord.xy,4).xyz * (1/32.0);
	color2 += textureLod(image, TexCoord.xy,5).xyz * (1/64.0);
	color2 += textureLod(image, TexCoord.xy,6).xyz * (1/128.0);
	color2 += textureLod(image, TexCoord.xy,7).xyz * (1/128.0);
	float i = clamp(getLum(color2),0,1);
	return mix(color1,color2,i);
}*/

vec3 nylon()
{
	vec3 color1 = texture(image, TexCoord.xy).xyz;
	vec3 color2 = color1 * (1/6.0);
	color2 += textureLod(image, TexCoord.xy,1).xyz * (1/6.0);
	color2 += textureLod(image, TexCoord.xy,2).xyz * (1/6.0);
	color2 += textureLod(image, TexCoord.xy,3).xyz * (1/6.0);
	color2 += textureLod(image, TexCoord.xy,4).xyz * (1/6.0);
	color2 += textureLod(image, TexCoord.xy,5).xyz * (1/6.0);
	float i = clamp(getLum(color2),0,1);
	return mix(color1,color2,i);
}

vec3 exposure(vec3 color)
{
	vec3 color2 = textureLod(image, vec2(0.5),20).xyz;
	float lum = getLum(color2);
	float expValue = 0.18/lum;
	//float expValue = 0.5/lum;
	return  1.0f - exp2(-color * expValue);
	//return color /* (1+color.a*64)*/ *expValue;
	//return color *expValue;
}

vec3 frame(vec3 exColor)
{
	vec2 distFromCenter = vec2(TexCoord * 2 - 1);
	//float d = length(distFromCenter) / sqrt(2.0); //1.4142135623730950488016887242097f;
	//float d = length(distFromCenter) / 1.4142135623730950488016887242097;
	//return mix(exColor,vec3(0),d*d);
	// d*d = length(distFromCenter)*length(distFromCenter) / (sqrt(2.0) * sqrt(2.0))
	// d*d = length(distFromCenter)^2 / 2
	// d*d = dot(distFromCenter,distFromCenter) / 2

	float d = dot(distFromCenter,distFromCenter) / 2;
	return mix(exColor,vec3(0),d);
}
/*
void main()
{ 
	// base color	
	vec3 color = nylon();
	//vec3 color = texture(image, TexCoord.xy).xyz;

	// exposure 
	color = exposure(color);

	// add frame
	color = frame(color);

	// apply gamma
	color = clamp(pow( color,vec3(1/2.2)),0,1-1/32.0);
	
	// apply lut
	color = texture(lut,color,0).xyz;

	// end
	gl_FragData.xyz = color;
	gl_FragData.w = getLum( gl_FragData.xyz );
}
*/
