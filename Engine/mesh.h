#pragma once

#include "vector.h"
#include "sphere.h"
#include <vector>
#include "collisiontriangle.h"

class CMesh
{
public:
	CMesh();
	virtual ~CMesh();
	
	void Init(int nVertices,int nIndices,bool normal=false,bool color = false,bool texture = false);
	void SetNormal(int idx,const float3& normal);
	void SetTexture(int idx,const float2& texture);
	void SetColor(int idx,const float4& color);
	void SetPoint(int idx,const float3& point);
	void SetIndex(int idx,int vidx);
	bool UpdateBuffers();
	void RemoveCPUCopy();

	bool LoadRaw(const char* i_strFilename);
	bool SaveRaw(const char* i_strFilename);

	void CalculateBoundings();
	void CalculateMinimalSphere();

	//it is tha same than call Predraw and ReDraw
	void Draw() const;

	void PreDraw() const;
	void ReDraw() const;
	static void EndDraw();

	float LineCollision(const float3& start,const float3& dir,float len = FLT_MAX);
	const floatSphere& GetBoundingSphere() const { return m_f4BoundingSphere; }
	const float3& GetMinPoint() const { return m_f3MinPoint; }
	const float3& GetMaxPoint() const { return m_f3MaxPoint; }

private:
	// VERTEX FORMAT
	bool m_bNormals;
	bool m_bColors;
	bool m_bTexCoords;
	
	unsigned int m_nVertices;
	unsigned int m_nIndices;

	unsigned int m_nFloatPerVertex;
	unsigned int m_iNormalDelta;
	unsigned int m_iColorDelta;
	unsigned int m_iTexCoordDelta;
	bool m_bUse16bit;

	// CPU COPY
	float *m_pVerticesArray;
	union
	{
		unsigned short *m_pIndices16;
		unsigned long *m_pIndices32;
		void* m_pIndicesVoid;
	};
	// GPU COPY
	unsigned int m_glVBuffer;
	unsigned int m_glIBuffer;
	// Extra data
	std::vector<CCollisionTriangle> m_aoCollisionTriangles;
	floatSphere m_f4BoundingSphere;
	float3 m_f3MinPoint;
	float3 m_f3MaxPoint;
};
