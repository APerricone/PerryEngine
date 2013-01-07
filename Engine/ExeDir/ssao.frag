#version 150

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform float intensity;
// near,far,radius
uniform vec3 zValues;

uniform mat4 projMatrix;
uniform mat4 invProjMatrix;

in vec2 TexCoord;
out vec4 gl_FragData;

vec3 GetPosWS(vec3 posSS)
{	
	vec4 nPosSS = vec4( (posSS*vec3(2))-vec3(1),1);

	vec4 posWS = invProjMatrix * nPosSS;
	posWS /= posWS.w;
	return posWS.xyz;
}

vec3 GetPosPS(vec3 posWS)
{	
	vec4 posSS = projMatrix * vec4(posWS,1);
	posSS /= posSS.w;
	return (posSS.xyz+vec3(1))*vec3(0.5);
}

float realDepth(float zBuf)
{
	float n = zValues.x;
	float f = zValues.y;
	float lin = (2.0 * n) / ((f + n) - zBuf * (f - n));
	return n + (lin * (f - n));
}

float GetDepth(vec2 p)
{
	return realDepth(texture(depthTexture, p).x);
}

//#define USE_T_B_SS 
float GetA(vec3 pos,float r,vec3 t,vec3 b,float alpha,float fact)
{
#ifdef USE_T_B_SS
	vec3 pSS = pos+(t * sin(alpha) + b * cos(alpha)) * r;
	float zPlane = pSS.z;
#else
	vec3 thispos = pos+(t * sin(alpha) + b * cos(alpha)) * r; 
	vec3 pSS = GetPosPS(thispos);
	float zPlane = realDepth(pSS.z);
#endif
	float zBuff = GetDepth(pSS.xy);
	float zDiff = zPlane - zBuff;
	if( zDiff>r*0.1 && zDiff<r*10 )
	{
		return min(zDiff,r)/(r*fact);
		//a-=1/(160.);
	}
	return 0.;
}

#define M_PI 3.14159265
float AA_big(vec3 pos,vec3 t,vec3 b,float rr)
{
	float a = 1;
	a -= GetA(pos,rr,t,b,0 * M_PI / 4, 28.);
	a -= GetA(pos,rr,t,b,1 * M_PI / 4, 28.);
	a -= GetA(pos,rr,t,b,2 * M_PI / 4, 28.);
	a -= GetA(pos,rr,t,b,3 * M_PI / 4, 28.);
	a -= GetA(pos,rr,t,b,5 * M_PI / 4, 28.);
	a -= GetA(pos,rr,t,b,6 * M_PI / 4, 28.);
	a -= GetA(pos,rr,t,b,7 * M_PI / 4, 28.);
	a -= GetA(pos,rr * 0.8,t,b,0 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.8,t,b,1 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.8,t,b,2 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.8,t,b,3 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.8,t,b,4 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.8,t,b,5 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.6,t,b,0 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.6,t,b,1 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.6,t,b,2 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.6,t,b,3 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.6,t,b,4 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.6,t,b,5 * M_PI / 3, 28.);
	a -= GetA(pos,rr * 0.4,t,b,0 * M_PI / 2, 28.);
	a -= GetA(pos,rr * 0.4,t,b,1 * M_PI / 2, 28.);
	a -= GetA(pos,rr * 0.4,t,b,2 * M_PI / 2, 28.);
	a -= GetA(pos,rr * 0.4,t,b,3 * M_PI / 2, 28.);
	a -= GetA(pos,rr * 0.2,t,b,0 * M_PI / 2, 28.);
	a -= GetA(pos,rr * 0.2,t,b,1 * M_PI / 2, 28.);
	a -= GetA(pos,rr * 0.2,t,b,2 * M_PI / 2, 28.);
	a -= GetA(pos,rr * 0.2,t,b,3 * M_PI / 2, 28.);
	return a;
}
float AA_little(vec3 pos,vec3 t,vec3 b,float rr)
{
	float a = 1;
	a -= GetA(pos,rr,t,b,0 * M_PI / 4, 18.);
	a -= GetA(pos,rr,t,b,1 * M_PI / 4, 18.);
	a -= GetA(pos,rr,t,b,2 * M_PI / 4, 18.);
	a -= GetA(pos,rr,t,b,3 * M_PI / 4, 18.);
	a -= GetA(pos,rr,t,b,5 * M_PI / 4, 18.);
	a -= GetA(pos,rr,t,b,6 * M_PI / 4, 18.);
	a -= GetA(pos,rr,t,b,7 * M_PI / 4, 18.);
	a -= GetA(pos,rr * 0.66,t,b,0 * M_PI / 3, 18.);
	a -= GetA(pos,rr * 0.66,t,b,1 * M_PI / 3, 18.);
	a -= GetA(pos,rr * 0.66,t,b,2 * M_PI / 3, 18.);
	a -= GetA(pos,rr * 0.66,t,b,3 * M_PI / 3, 18.);
	a -= GetA(pos,rr * 0.66,t,b,4 * M_PI / 3, 18.);
	a -= GetA(pos,rr * 0.66,t,b,5 * M_PI / 3, 18.);
	a -= GetA(pos,rr * 0.33,t,b,0 * M_PI / 2, 18.);
	a -= GetA(pos,rr * 0.33,t,b,1 * M_PI / 2, 18.);
	a -= GetA(pos,rr * 0.33,t,b,2 * M_PI / 2, 18.);
	a -= GetA(pos,rr * 0.33,t,b,3 * M_PI / 2, 18.);
	return a;
}

void main()
{ 	
	float zPos = texture(depthTexture, TexCoord.xy).x;
	vec3 posS = vec3(TexCoord,zPos);
	vec3 pos = GetPosWS(posS);
	vec3 normal = normalize( (texture(normalTexture, TexCoord.xy).xyz* vec3(2))-vec3(1) );
	vec3 t = vec3(0,0,0); 
	if(abs(normal.y)>0.9) t.x = 1; else t.y = 1;
	vec3 b = normalize(cross(t,normal));
	t = normalize(cross(b,normal));

#ifdef USE_T_B_SS
	posS.z = realDepth(zPos);
	vec3 t1 = pos+t;
	t = GetPosPS(t1);
	t.z = realDepth(t.z);
	t -= posS;
	vec3 b1 = pos+t;
	b = GetPosPS(b1);
	b.z = realDepth(b.z);
	b -= posS;
	pos = posS;
#endif
	
	float aa1 = 1;//AA_big(pos,t,b,1.0);
	float aa2 = AA_little(pos,t,b,0.1);
	gl_FragData = vec4(1,1,1,0) * aa1 * aa2 * intensity;
	gl_FragData.w = 1;
}
