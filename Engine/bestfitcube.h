#pragma once
#include "vector.h"

typedef V3<unsigned char> uchar3;

class CBestFitCube
{
public:
	CBestFitCube();
	~CBestFitCube();

	void Init(unsigned int iSize);

	unsigned int m_glTexture;

private:
	unsigned int m_iSize;

	unsigned char* faces[6];

	float3* m_pTempConversionTable;
	void CreateConversionTable();
	void FillFaces();

	uchar3 GetBestFit(float3& iPixel);
	void SetValueX6(const uint3& pixel,const uchar3& Value);
	void SetPixel(unsigned int f,unsigned int x,unsigned int y,const uchar3& Value);

	static float3 Convert(const uchar3& iPixel);
};
