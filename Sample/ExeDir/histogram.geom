#version 150

layout(points) in;
layout(points,max_vertices = 128) out;

uniform sampler2D image;
// x: max value for lum
uniform float fHistogramParameter;

// pixel value
in vec4 texCoords[];
flat out uvec4 color;

float getLum(vec3 c)
{
	return dot(c, vec3(0.299, 0.587, 0.114) );
}

void main()
{
	float maxC = fHistogramParameter;
	uint h[512];
	for(int i=0;i<512;i++) h[i] = 0u;
	ivec2 texSize = textureSize(image,0);
	ivec2 s = ivec2(texCoords[0].xy*texSize);
	ivec2 e = ivec2(texCoords[0].zw*texSize);
	ivec2 cnt = s;
	//for(cnt.y=s.y;cnt.y<e.y;cnt.y++)
	//	for(cnt.x=s.y;cnt.x<s.x;cnt.x++)
	{
		float c = getLum(texelFetchOffset(image,s,0,ivec2(cnt)).xyz);
		int d = int(clamp(c,0,maxC)*512/maxC);
		h[d]++;
	}
	gl_Position.yzw = vec3(0,0,1);
	//for(int i=0;i<512;i+=4)
	int i=0;
	{
		int px = i/4; //0<px<128
		gl_Position.x = -1 + px / 64.;   //2./ 128.;
		color = uvec4(h[i+0],h[i+1],h[i+2],h[i+3]);
		bvec4 bRnd = greaterThan(color, vec4(0));
		if( all(bRnd) )
		{
			EmitVertex();
			EndPrimitive();
		}
	}
}
