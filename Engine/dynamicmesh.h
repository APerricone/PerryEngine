#pragma once

#include "vector.h"
#include <map>
#include <vector>
#include "mesh.h"

class CDynamicMesh
{
public:
	CDynamicMesh();
	virtual ~CDynamicMesh();

	void Clear();

	void Normal(const float3& normal);
	void Texture(const float2& texture);
	void Color(const float4& color);
	void Point(const float3& point);

	void AddSphere(const float3& center,const float radius,int nSegment=8,int nDivision=16,int startSegment=0,int endSegment=-1);
	void AddBox(const float3& min,const float3& max);
	void AddCone(	const float3& center1,const float radius1,
					const float3& center2,const float radius2,int nDivision=16,bool base1=true,bool base2=true);
	void AddTorus(const float3& center,const float radius1,const float radius2,int nSegment=8,int nDivision=16);

	void CalculateNormals();

	void CreateMesh(CMesh& oDest,bool normal=false,bool color = false,bool texture = false);

private:
	struct InputPoint
	{
		float3 pos;
		float3 nor;
		float2 tex;
		float4 col;
	};
	typedef std::vector<InputPoint> PointList;
	typedef std::multimap<float,unsigned int> PointMap;
	typedef std::vector<unsigned int> TriangleList;
	PointMap mappedPoints;
	PointList points;
	TriangleList triangles;

	float3 currentNormal;
	float2 currentTexture;
	float4 currentColor;

	unsigned int FindVertex(const InputPoint &v);
	void AddQuadForBox(const float3& a,const float3& b,const float3& c,const float3& d,
		const float2& texmin=float2(0,0),const float2& texmax=float2(1,1));
};
