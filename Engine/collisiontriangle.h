#pragma once

#include "vector.h"
#include "float.h"

class CCollisionTriangle
{
public:
	CCollisionTriangle(const float3& a,const float3& b,const float3& c);

	float LineCollision(const float3& p,const float3& d,float len = FLT_MAX);

private:
	float4 m_f4ContainerPlane;
	float4 m_f4LateralPlanes[3];
};

