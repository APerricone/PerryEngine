#define M_PI 3.14159265
#define N_DIREX 8
#define N_STEP 8

uniform vec2 zValues;
#define NEAR_PLANE zValues.x
#define LIGHT_WORLD_SIZE 2. 
#define LIGHT_FRUSTUM_WIDTH 3.75 
// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT 
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)

float realDepth(float zBuf)
{
	float n = zValues.x;
	float f = zValues.y;
	float lin = (2.0 * n) / (f + n - zBuf * (f - n));
	return n + (lin * (f - n));
}

float PenumbraSize(float zReceiver, float zBlocker) //Parallel plane estimation
{ 
	return (zReceiver - zBlocker) / zBlocker; 
}

int FindBlocker(out float avgBlockerDepth, vec2 uv, float zReceiver )
{
	//This uses similar triangles to compute what  
	//area of the shadow map we should search 
	float searchWidth = LIGHT_SIZE_UV * (zReceiver - NEAR_PLANE) / zReceiver; 
	float blockerSum = 0; 
	int numBlockers = 0; 
	for(int i=0;i<N_DIREX;i++)
	{
		float alpha = i * M_PI * (2. / N_DIREX);
		vec2 direx = vec2(sin(alpha),cos(alpha)); 
		for(int j=1;j<=N_STEP;j++)
		{
			float f = searchWidth * j / float(N_STEP);
			float zBlocker = realDepth(texture( shadowTexture, (uv + direx * f) ).x);
			if ( zBlocker < zReceiver )
			{ 
				blockerSum += zBlocker; 
				numBlockers++; 
			} 
		} 
	} 
	//*
    avgBlockerDepth = blockerSum / float(numBlockers);
	return numBlockers;
	/* /
	avgBlockerDepth = searchWidth;
	return 1; 
	//*/
}

float PCF_Filter( vec2 uv, float zReceiver, float filterRadiusUV ) 
{ 
	float sum = 0.0f; 
	for(int i=0;i<N_DIREX;i++)
	{
		float alpha = i * M_PI * (2. / N_DIREX);
		vec2 direx = vec2(sin(alpha),cos(alpha)); 
		for(int j=1;j<=N_STEP;j++)
		{
			float f = filterRadiusUV * j / float(N_STEP);
			vec2 offset = direx * f; 
			float zBlocker = texture( shadowTexture, (uv + offset) ).x;
			if( zBlocker > zReceiver )
				sum += 1;
		}
	}
	return sum / float(N_DIREX*N_STEP);
} 

float Shadow(vec4 posWS)
{
	vec4 posLS = (lightMatrix * posWS);
	posLS /= posLS.w;
	vec3 texC = ((posLS.xyz + 1) * 0.5);
	
	vec2 uv = texC.xy; 
	float zReceiver = realDepth(texC.z); // Assumed to be eye-space z in this code

    // STEP 1: blocker search 
	float avgBlockerDepth = 0; 
    if( FindBlocker( avgBlockerDepth, uv, zReceiver ) < 1 )   
		//There are no occluders so early out (this saves filtering) 
		return 1.0; 
    // STEP 2: penumbra size 
	float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);     
	
	float filterRadiusUV = penumbraRatio * LIGHT_SIZE_UV * NEAR_PLANE / zReceiver; 
	// STEP 3: filtering 
	return PCF_Filter( uv, texC.z, filterRadiusUV ); 
	//return filterRadiusUV; //*/
}
