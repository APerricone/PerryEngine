#include "dynamicmesh.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "mymacros.h"
#include "log.h"

#include <stdio.h>
#include "opengl.h"

CDynamicMesh::CDynamicMesh()
{
	currentNormal = float3(0,0,0);
	currentTexture = float2(0,0);
	currentColor = float4(1,1,1,1);
}

CDynamicMesh::~CDynamicMesh()
{
	points.clear();
	mappedPoints.clear();
	triangles.clear();
}

void CDynamicMesh::Clear()
{
	currentNormal = float3(0,0,0);
	currentTexture = float2(0,0);
	currentColor = float4(1,1,1,1);
	points.clear();
	mappedPoints.clear();
	triangles.clear();
}

void CDynamicMesh::Normal(const float3& normal)
{
	currentNormal= normal;
}

void CDynamicMesh::Texture(const float2& texture)
{
	currentTexture = texture;
}

void CDynamicMesh::Color(const float4& color)
{
	currentColor = color;
}

void CDynamicMesh::Point(const float3& point)
{
	InputPoint tmp;
	tmp.pos = point;
	tmp.nor = currentNormal;
	tmp.col = currentColor;
	tmp.tex = currentTexture;
	triangles.push_back( FindVertex(tmp) );
}

unsigned int CDynamicMesh::FindVertex(const InputPoint &v)
{
	float key = v.pos.GetLen();
	PointMap::iterator l = mappedPoints.lower_bound(key-1e-3f);
	PointMap::iterator u = mappedPoints.upper_bound(key+1e-3f);
	for(PointMap::iterator i=l;i!=u;++i)
	{
		InputPoint& thisP = points[ (*i).second ];
		if( thisP.pos != v.pos ) continue;
		if( thisP.nor != v.nor ) continue;
		if( thisP.tex != v.tex ) continue;
		if( thisP.col != v.col ) continue;
		return (*i).second;
	}
	unsigned int index = points.size();
	points.push_back( v );
	mappedPoints.insert( std::pair<float,unsigned int>(key,index));
	return index;
}

void CDynamicMesh::AddSphere(const float3& center,const float radius,int nSegment,int nDivision,int segmentStart,int segmentEnd)
{
	if( segmentEnd == -1 ) segmentEnd = nSegment;
	float stepA = (float)(M_PI/nSegment);
	float stepB = (float)(M_PI*2/nDivision);
	float a = 0;
	float cosA[2] = { 1,1};
	float sinA[2] = { 0,0};
	float cosB[2] = { 1,1};
	float sinB[2] = { 0,0};
	for(int j=0;j<segmentStart;j++)
	{
		cosA[0] = cosA[1];
		sinA[0] = sinA[1];
		a+= stepA;
		cosA[1] = cosf( a );
		sinA[1] = sinf( a );
	}
	for(int j=segmentStart;j<segmentEnd;j++)
	{
		cosA[0] = cosA[1];
		sinA[0] = sinA[1];
		a+= stepA;
		cosA[1] = cosf( a );
		sinA[1] = sinf( a );
		float b = 0;
		cosB[1] = 1;
		sinB[1] = 0;
		for(int i=0;i<nDivision;i++)
		{
			cosB[0] = cosB[1];
			sinB[0] = sinB[1];
			b+= stepB;
			cosB[1] = cosf( b );
			sinB[1] = sinf( b );
			float3 normals[4];
			float3 points[4];
			for(int r=0;r<4;r++)
			{
				int ia = r/2;
				int ib = r%2;
				normals[r].x() = sinA[ia] * cosB[ib];
				normals[r].y() = cosA[ia];
				normals[r].z() = sinA[ia] * sinB[ib];
				points[r] = center + normals[r] * radius;
			}
			if(j!=0)
			{
				Normal( normals[0] );
				Point( points[0] );
				Normal( normals[1] );
				Point( points[1] );
				Normal( normals[2] );
				Point( points[2] );
			}
			if(j!=nSegment-1)
			{
				Normal( normals[2] );
				Point( points[2] );
				Normal( normals[1] );
				Point( points[1] );
				Normal( normals[3] );
				Point( points[3] );
			}
		}
	}
}

void CDynamicMesh::AddQuadForBox(const float3& a,const float3& b,const float3& c,const float3& d,
		const float2& t1,const float2& t2)
{
	Texture(float2(t1[0],t1[1])); Point(a);
	Texture(float2(t2[0],t1[1])); Point(b);
	Texture(float2(t1[0],t2[1])); Point(c);

	Texture(float2(t1[0],t2[1])); Point(c);
	Texture(float2(t2[0],t1[1])); Point(b);
	Texture(float2(t2[0],t2[1])); Point(d);
}

void CDynamicMesh::AddBox(const float3& min,const float3& max)
{
	Normal(float3(0,1,0));
	AddQuadForBox( 	float3( max[0], max[1], min[2]),
					float3( min[0], max[1], min[2]),
					float3( max[0], max[1], max[2]),
					float3( min[0], max[1], max[2]) );
	Normal(float3(0,-1,0));
	AddQuadForBox( 	float3( min[0], min[1], min[2]),
					float3( max[0], min[1], min[2]),
					float3( min[0], min[1], max[2]),
					float3( max[0], min[1], max[2]) );
	Normal(float3(1,0,0));
	AddQuadForBox( 	float3( max[0], min[1], min[2]),
					float3( max[0], max[1], min[2]),
					float3( max[0], min[1], max[2]),
					float3( max[0], max[1], max[2]) );
	Normal(float3(-1,0,0));
	AddQuadForBox( 	float3( min[0], max[1], min[2]),
					float3( min[0], min[1], min[2]),
					float3( min[0], max[1], max[2]),
					float3( min[0], min[1], max[2]) );
	Normal(float3(0,0,1));
	AddQuadForBox( 	float3( min[0], max[1], max[2]),
					float3( min[0], min[1], max[2]),
					float3( max[0], max[1], max[2]),
					float3( max[0], min[1], max[2]) );
	Normal(float3(0,0,-1));
	AddQuadForBox( 	float3( min[0], min[1], min[2]),
					float3( min[0], max[1], min[2]),
					float3( max[0], min[1], min[2]),
					float3( max[0], max[1], min[2]) );
}

void CDynamicMesh::AddCone(	const float3& center1,const float radius1,
					const float3& center2,const float radius2,int nDivision,bool base1,bool base2)
{
	float3 rg,up,at;
	at = (center2-center1); at.Normalize();
	if(fabsf(at[1])>0.9f) up.Set(0,0,1); else up.Set(0,1,0);
	rg = up ^ at; rg.Normalize();
	up = at ^ rg; up.Normalize();

	float cosA[2] = { 1,1 };
	float sinA[2] = { 0,0 };
	float texA[2] = { 0,0 };
	float a = 0;
	float step = (float)(M_PI*2/nDivision);
	for(int i=0;i<nDivision;i++)
	{
		cosA[0] = cosA[1];
		sinA[0] = sinA[1];
		texA[0] = texA[1];
		a+= step;
		cosA[1] = cosf( a );
		sinA[1] = sinf( a );
		texA[1] = (float)(i+1)/(float)nDivision;
		float3 normals[2];
		normals[0] = rg*cosA[0]+up*sinA[0];
		normals[1] = rg*cosA[1]+up*sinA[1];

		Texture(float2(texA[0],0));
		Normal(normals[0]);
		Point(center1 + normals[0] * radius1 );

		Texture(float2(texA[1],0));
		Normal(normals[1]);
		Point(center1 + normals[1] * radius1 );

		Texture(float2(texA[0],1));
		Normal(normals[0]);
		Point(center2 + normals[0] * radius2 );

		Texture(float2(texA[0],1));
		Normal(normals[0]);
		Point(center2 + normals[0] * radius2 );

		Texture(float2(texA[1],0));
		Normal(normals[1]);
		Point(center1 + normals[1] * radius1 );

		Texture(float2(texA[1],1));
		Normal(normals[1]);
		Point(center2 + normals[1] * radius2 );

		if(base1)
		{
			Normal(-at);

			Texture(float2(0.5f,0.5f));
			Point( center1 );

			Texture(float2(1+cosA[1],1+sinA[1])/2.f);
			Point( center1 + normals[1] * radius1 );

			Texture(float2(1+cosA[0],1+sinA[0])/2.f);
			Point( center1 + normals[0] * radius1 );
		}

		if(base2)
		{
			Normal(at);

			Texture(float2(0.5f,0.5f));
			Point( center2 );

			Texture(float2(1+cosA[0],1+sinA[0])/2.f);
			Point( center2 + normals[0] * radius2 );

			Texture(float2(1+cosA[1],1+sinA[1])/2.f);
			Point( center2 + normals[1] * radius2 );
		}
	}
}

void CDynamicMesh::CalculateNormals()
{
	std::vector<double3> af3TriNormals;
	af3TriNormals.resize( points.size(), double3(0.) );
	unsigned int t;
	for(t=0;t<triangles.size();t+=3)
	{
		double3 ab = double3(points[triangles[t+1]].pos-points[triangles[ t ]].pos);
		double3 ac = double3(points[triangles[t+2]].pos-points[triangles[ t ]].pos);
		double3 bc = double3(points[triangles[t+2]].pos-points[triangles[t+1]].pos);
		double3 n = (ab^ac);
		n.Normalize();
		double a = sqrt(dot(ab,ab));
		double b = sqrt(dot(bc,bc));
		double c = sqrt(dot(ac,ac));
		double p = (a+b+c)/2.;
		// Heron's formula
		double area = sqrt( p*(p-a)*(p-b)*(p-c) );
		n*=area;
		af3TriNormals[triangles[t+0]] += n;
		af3TriNormals[triangles[t+1]] += n;
		af3TriNormals[triangles[t+2]] += n;
	}
	unsigned int i;
	for(i=0;i<points.size();i++)
	{
		double3 n(af3TriNormals[i]);
		n.Normalize();
		points[i].nor = float3(float(n[0]),float(n[1]),float(n[2]));
	}
}

void CDynamicMesh::CreateMesh(CMesh& oDest,bool _normal,bool _color,bool _texture)
{
	int nIgnored = (triangles.size()%3);
	if( nIgnored != 0)
	{
		ILog::Error("CDynamicMesh::End: wrong number of vertices: ignored %i vertices\n",nIgnored);
		triangles.resize( triangles.size() - nIgnored );
	}
	oDest.Init(points.size(),triangles.size(),_normal,_color,_texture);
	for(unsigned int i=0;i<triangles.size();i++)
	{
		oDest.SetIndex(i,triangles[i]);
	}
	for(unsigned int i=0;i<points.size();i++)
	{
		oDest.SetPoint(i,points[i].pos);
		oDest.SetNormal(i,points[i].nor);
		oDest.SetColor(i,points[i].col);
		oDest.SetTexture(i,points[i].tex);
	}
	
	oDest.UpdateBuffers();
	oDest.CalculateBoundings();
}
