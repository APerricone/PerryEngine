#include "bestfitcube.h"

#include "opengl.h"
#include "mymacros.h"
#include "log.h"

#include <float.h>
#include <stdio.h>

#define QUICK_START
#ifdef _DEBUG
#define QUICK_START
#endif

CBestFitCube::CBestFitCube()
{
	m_glTexture = 0;
}

CBestFitCube::~CBestFitCube()
{
	SAFE_DELETE_GL_TEXTURE(m_glTexture);
}

void progress(const char* message,unsigned char v)
{
	ILog::Message("\r\t%s [%c%c%c%c%c%c%c]                                   ",message,
		v>=1? '*' : '.', v>=2? '*' : '.', v>=3? '*' : '.', v>=4? '*' : '.',
		v>=5? '*' : '.', v>=6? '*' : '.', v>=7? '*' : '.');
}

void CBestFitCube::Init(unsigned int iSize)
{
	ILog::Message("\ncreating cube...\n");

	CreateConversionTable();
	m_iSize = iSize;
	for(int f=0;f<6;f++)
		faces[f] = new unsigned char[iSize*iSize*4];
	
	FillFaces();	

	ILog::Message("\r\tDone                                                               \n");
	glGenTextures(1,&m_glTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP,m_glTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,0,3,iSize,iSize,0,GL_RGB,GL_UNSIGNED_BYTE,faces[0]);	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,0,3,iSize,iSize,0,GL_RGB,GL_UNSIGNED_BYTE,faces[1]);	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,0,3,iSize,iSize,0,GL_RGB,GL_UNSIGNED_BYTE,faces[2]);	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,0,3,iSize,iSize,0,GL_RGB,GL_UNSIGNED_BYTE,faces[3]);	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,0,3,iSize,iSize,0,GL_RGB,GL_UNSIGNED_BYTE,faces[4]);	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,0,3,iSize,iSize,0,GL_RGB,GL_UNSIGNED_BYTE,faces[5]);	

	glBindTexture(GL_TEXTURE_CUBE_MAP,0);

	for(int f=0;f<6;f++)
		delete [] faces[f];
	delete [] m_pTempConversionTable;
}


void CBestFitCube::CreateConversionTable()
{
	m_pTempConversionTable = new float3[256*256*256];
#ifndef QUICK_START
	unsigned char v = 0;
	uint3 k;
	float err = FLT_MAX;
	for(k[0] = 0; k[0]<256; k[0]++)
	{
		progress("creating conversion table",k[0]>>5);
		for(k[1] = 0; k[1]<256; k[1]++)
			for(k[2] = 0; k[2]<256; k[2]++)
			{
				m_pTempConversionTable[k[0]+(k[1]+k[2]*256)*256] = Convert(uchar3(k));
				m_pTempConversionTable[k[0]+(k[1]+k[2]*256)*256].Normalize();
			}
	}
#endif //_DEBUG
}

void CBestFitCube::FillFaces()
{
	unsigned int shift = ilog2(m_iSize) - 4;
	uint3 i((unsigned int)0);
	for(i[0]=0;i[0]<m_iSize/2;i[0]++)
	{
		progress("filling value table",i[0]>>shift);
		for(i[1]=0;i[1]<m_iSize/2;i[1]++)
		{
			float3 v;
			v[0] = ( ( float(i[0]) + 0.5f ) / float(m_iSize) ) * 2.f - 1.f;
			v[1] = ( ( float(i[1]) + 0.5f ) / float(m_iSize) ) * 2.f - 1.f;
			v[2] = -1.f;
			SetValueX6(i,GetBestFit(v));
		}
	}	
}

uchar3 CBestFitCube::GetBestFit(float3& iNormal)
{
#ifndef QUICK_START
	uchar3 r;
	uint3 i;
	float err = FLT_MAX;
	for(float d=0;d<128;d+=0.5f)
	{
		i = uint3( float3(127.5f) + iNormal * d);

		float3 n = m_pTempConversionTable[i[0]+(i[1]+i[2]*256)*256];
		//float3 n = Convert(uchar3(i));  n.Normalize();
		float tmpErr = dot((n-iNormal),(n-iNormal));
		if( tmpErr<err)
		{
			r = uchar3(i);
			err = tmpErr;
		}
		
	}
	/*
	for(i[0] = 0; i[0]<256; i[0]++)
		for(i[1] = 0; i[1]<256; i[1]++)
			for(i[2] = 0; i[2]<256; i[2]++)
	{
		float3 n = m_pTempConversionTable[i[0]+(i[1]+i[2]*256)*256];
			//Convert(uchar3(i));
		float d = dot((n-iNormal),(n-iNormal));
		if( d<err)
		{
			r = uchar3(i);
			err = d;
		}
	}*/
	return r;
#else //QUICK_START
	return uchar3(
			(unsigned char)(((iNormal[0]+1.f) * 0.5f) * 255.f),
			(unsigned char)(((iNormal[1]+1.f) * 0.5f) * 255.f),
			(unsigned char)(((iNormal[2]+1.f) * 0.5f) * 255.f)
		);
#endif //QUICK_START

}

void CBestFitCube::SetValueX6(const uint3& pixel,const uchar3& value)
{
	// 0: -rz -ry +rx
	SetPixel(0,      pixel[0]     ,      pixel[1]     ,	uchar3(255-value[2],255-value[1],255-value[0]));
	SetPixel(0,      pixel[0]     , m_iSize-1-pixel[1],	uchar3(255-value[2],  value[1]  ,255-value[0]));
	SetPixel(0, m_iSize-1-pixel[0],      pixel[1]     , uchar3(255-value[2],255-value[1],  value[0]  ));
	SetPixel(0, m_iSize-1-pixel[0], m_iSize-1-pixel[1], uchar3(255-value[2],  value[1]  ,  value[0]  ));
	// 1: +rz -ry -rx
	SetPixel(1,      pixel[0]     ,      pixel[1]     ,	uchar3(  value[2]  ,255-value[1],  value[0]  ));
	SetPixel(1,      pixel[0]     , m_iSize-1-pixel[1],	uchar3(  value[2]  ,  value[1]  ,  value[0]  ));
	SetPixel(1, m_iSize-1-pixel[0],      pixel[1]     , uchar3(  value[2]  ,255-value[1],255-value[0]));
	SetPixel(1, m_iSize-1-pixel[0], m_iSize-1-pixel[1], uchar3(  value[2]  ,  value[1]  ,255-value[0]));
	// 2: +rx +rz +ry
	SetPixel(2,      pixel[0]     ,      pixel[1]     ,	uchar3(  value[0]  ,255-value[2],  value[1]  ));
	SetPixel(2,      pixel[0]     , m_iSize-1-pixel[1],	uchar3(  value[0]  ,255-value[2],255-value[1]));
	SetPixel(2, m_iSize-1-pixel[0],      pixel[1]     , uchar3(255-value[0],255-value[2],  value[1]  ));
	SetPixel(2, m_iSize-1-pixel[0], m_iSize-1-pixel[1], uchar3(255-value[0],255-value[2],255-value[1]));
	// 3: +rx -rz -ry
	SetPixel(3,      pixel[0]     ,      pixel[1]     ,	uchar3(  value[0]  ,  value[2]  ,255-value[1]));
	SetPixel(3,      pixel[0]     , m_iSize-1-pixel[1],	uchar3(  value[0]  ,  value[2]  ,  value[1]  ));
	SetPixel(3, m_iSize-1-pixel[0],      pixel[1]     , uchar3(255-value[0],  value[2]  ,255-value[1]));
	SetPixel(3, m_iSize-1-pixel[0], m_iSize-1-pixel[1], uchar3(255-value[0],  value[2]  ,  value[1]  ));
	// 4: +rx -ry +rz
	SetPixel(4,      pixel[0]     ,      pixel[1]     ,	uchar3(  value[0]  ,255-value[1],255-value[2]));
	SetPixel(4,      pixel[0]     , m_iSize-1-pixel[1],	uchar3(  value[0]  ,  value[1]  ,255-value[2]));
	SetPixel(4, m_iSize-1-pixel[0],      pixel[1]     , uchar3(255-value[0],255-value[1],255-value[2]));
	SetPixel(4, m_iSize-1-pixel[0], m_iSize-1-pixel[1], uchar3(255-value[0],  value[1]  ,255-value[2]));
	// 5: -rx -ry -rz
	SetPixel(5,      pixel[0]     ,      pixel[1]     ,	uchar3(255-value[0],255-value[1],  value[2]  ));
	SetPixel(5,      pixel[0]     , m_iSize-1-pixel[1],	uchar3(255-value[0],  value[1]  ,  value[2]  ));
	SetPixel(5, m_iSize-1-pixel[0],      pixel[1]     , uchar3(  value[0]  ,255-value[1],  value[2]  ));
	SetPixel(5, m_iSize-1-pixel[0], m_iSize-1-pixel[1], uchar3(  value[0]  ,  value[1]  ,  value[2]  ));
}

void CBestFitCube::SetPixel(unsigned int f,unsigned int x,unsigned int y,const uchar3& value)
{
	unsigned char *d = &(faces[f][(x+y*m_iSize)*3]);
	d[0] = value[0];
	d[1] = value[1];
	d[2] = value[2];
}

float3 CBestFitCube::Convert(const uchar3& iPixel)
{
	return float3(
		(float(iPixel[0]) / float(255)) * 2.f - 1.f,
		(float(iPixel[1]) / float(255)) * 2.f - 1.f,
		(float(iPixel[2]) / float(255)) * 2.f - 1.f );
}
