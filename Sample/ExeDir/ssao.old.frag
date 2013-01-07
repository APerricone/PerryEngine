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
	float lin = (2.0 * n) / (f + n - zBuf * (f - n));
	return n + (lin * (f - n));
}

float GetDepth(vec2 p)
{
	return realDepth(texture(depthTexture, p).x);
}

#define M_PI 3.14159265
#define N_DIREX 8
#define N_STEP 5
float AA(vec3 posS,vec3 pos,vec3 t,vec3 b,float rr)
{
	float startZ = realDepth(posS.z);
	float a = 1;
	for(int i=0;i<N_DIREX;i++)
	{
		float alpha = i * M_PI * (2. / N_DIREX);
		vec3 direx = (t * sin(alpha) + b * cos(alpha)) * rr; 
		vec3 thisPos = pos + direx;
		vec3 thisSS = GetPosPS(thisPos);
		for(int j=1;j<=N_STEP;j++)
		{
			float f = j / float(N_STEP);
			vec3 pSS = mix(posS,thisSS, f);
			float zPlane = realDepth(pSS.z);
			float zBuff = GetDepth(pSS.xy);
			float zDiff = zPlane - zBuff;
			float r = rr * f;
			if( zDiff>r*0.1 && zDiff<r*10 )
			{
				a-=min(zDiff,r)/(r*(N_DIREX*N_STEP));
				//a-=1/(160.);
			}
		}
	}
	return a;
}

void main()
{ 	
	float zPos = texture(depthTexture, TexCoord.xy).x;
	if( zPos > 0.99 ) discard;
	vec3 posS = vec3(TexCoord,zPos);
	vec3 pos = GetPosWS(posS);
	vec3 normal = normalize( (texture(normalTexture, TexCoord.xy).xyz* vec3(2))-vec3(1) );
	vec3 t = vec3(0,0,0); 
	if(abs(normal.y)>0.9) t.x = 1; else t.y = 1;
	vec3 b = normalize(cross(t,normal));
	t = normalize(cross(b,normal));
	
	//float aa1 = AA(pos,t,b,zValues.z);
	//float aa2 = 1; //AA(pos,t,b,zValues.z*2);
	float aa1 = AA(posS,pos,t,b,1.0);
	float aa2 = AA(posS,pos,t,b,0.1);
	gl_FragData = vec4(1,1,1,0) * aa1 * aa2 * intensity;
	gl_FragData.w = 1;
}

