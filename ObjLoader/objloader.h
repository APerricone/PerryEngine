#pragma once

#include <vector>
#include "vector.h"
#include "dynamicmesh.h"
#include "material.h"

class CModel;

class CObjLoader
{
public:
	~CObjLoader();

	bool LoadObj(const char* i_strFilename);

private:

	const char* m_strFilename;
	char m_strCurrLine[1024];
	//
	CModel* m_pDest;
	CDynamicMesh m_oCreator;
	std::vector<CMaterial::Data> m_aMaterials;
	std::vector<char*> m_astrMaterialNames;
	// 
	bool m_bCurrHasNormal;
	bool m_bCurrHasTextures;
	std::vector<float3> m_af3Positions;
	std::vector<float3> m_af3normals;
	std::vector<float2> m_af2textures;
	//
	// main sub procedure
	bool ParseObj();
	bool ParseMtl();
	// 
	bool ManageObj_v();
	bool ManageObj_o();
	bool ManageObj_u();
	bool ManageObj_f();
	bool ManageObj_g();
	void CloseMesh();
	// 
	void ManageMtl_K();
	void ManageMtl_N();
	void ManageMtl_m();
	void ManageMtl_n();
};
