#pragma once

#include "node.h"
#include "vector.h"
#include "material.h"
#include "float.h"

class CMesh;

// THERE IS NO WAY TO REMOVE MESHES OR MATERIAL FROM A MODEL!
class CModel : public CNode
{
	using CNode::m_iClassId;
	CModel();
public:
	static CModel* CreateModel();
	virtual ~CModel();

	static void CheckVisibilities(const float4 i_af4Frustum[6]);

	//! 
	void ReserveMeshes(unsigned int i_nHowMany);
	//!
	unsigned int AddMesh();
	//! \note this pointer will be invalid on meshes resize: don't cache it!
    CMesh* GetMesh(unsigned int i) { if( i>=m_apMeshes.size() ) return 0; else return m_apMeshes[i]; }
	//! \note this pointer will be invalid on meshes resize: don't cache it!
    CMesh* GetLastMesh() { if( m_apMeshes.empty() ) return 0; else return m_apMeshes.back(); }
	//!
	void MeshChanged() { m_bNeedUpdateBS = true; }
	//!
	void SetMaterial(unsigned int i,CMaterial::Data& i_oMaterial);
	//!
	void SetLastMaterial(CMaterial::Data& i_oMaterial) { SetMaterial(m_apMeshes.size()-1,i_oMaterial); }

	bool IsKindModel() { return (m_iClassId & 1)!=0; }
	bool IsExatlyModel() { return (m_iClassId)==1; }

	virtual float LineCollision(const float3& start,const float3& dir,float len = FLT_MAX);

	virtual void Draw(bool i_bIgnoreVisibility, bool i_bSetMaterial);

	const floatSphere& GetBoundingSphere() const { return s_af4BoundingSpheres[m_iVisibility]; }
protected:
	std::vector<CMesh*> m_apMeshes;
	std::vector<CMaterial::Data> m_aoMaterial;
	unsigned int m_iVisibility;

	static std::vector<floatSphere> s_af4BoundingSpheres;
	static std::vector<bool> s_abVisible;

	bool m_bNeedUpdateBS;

	static void SortBoundings();
	static void UpdateBoundings();
	static bool s_bAddedToUpdates;


	struct SortBoundingFunctor
	{
		void operator()(CNode*);
		unsigned int m_iCount;
	};
	struct UpdateBoundingsFunctor
	{
		void operator()(CNode*);
	};
	void UpdateBounding();
};
