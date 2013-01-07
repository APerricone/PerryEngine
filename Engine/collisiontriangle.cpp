#include "collisiontriangle.h"

CCollisionTriangle::CCollisionTriangle(const float3& a,const float3& b,const float3& c)
{
	float3 n = ( (float3)(b-a)^(float3)(c-a) ); n.Normalize();
	float3 nab = ( n^(float3)(b-a) ); nab.Normalize();
	float3 nbc = ( n^(float3)(c-b) ); nbc.Normalize();
	float3 nca = ( n^(float3)(a-c) ); nca.Normalize();
	
	m_f4ContainerPlane.Set( n, -dot(n,a) );
	m_f4LateralPlanes[0].Set( nab, -dot(nab,a) );
	m_f4LateralPlanes[1].Set( nbc, -dot(nbc,b) );
	m_f4LateralPlanes[2].Set( nca, -dot(nca,c) );
}

float CCollisionTriangle::LineCollision(const float3& start,const float3& dir,float len)
{
	float den=dot(float4(dir,0),m_f4ContainerPlane);
	if(fabsf(den)>1e-3f)
	{
		float t = -(dot(float4(start,1),m_f4ContainerPlane))/den;
		if(t>1e-3f && t<len)
		{
			float4 point(start + dir * t,1);
			if(	dot(point,m_f4LateralPlanes[0])>-1e-3f &&
				dot(point,m_f4LateralPlanes[1])>-1e-3f &&
				dot(point,m_f4LateralPlanes[2])>-1e-3f)
			{
				//pos = point;
				//nor = make_float3(m_f4ContainerPlane);
				return t;
			}
		}
	}
	return FLT_MAX;
}

