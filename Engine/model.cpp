#include "model.h"
#include "opengl.h"
#include "mesh.h"
#include "material.h"

CModel::CModel() : CNode()
{
	if(!s_bAddedToUpdates)
	{
		s_afUpdatePasses.push_back(SortBoundings);
		s_afUpdatePasses.push_back(UpdateBoundings);
		s_bAddedToUpdates = true;
	}
	m_iVisibility = s_af4BoundingSpheres.size();

	s_af4BoundingSpheres.push_back(floatSphere());
	s_abVisible.push_back(true);
	m_bNeedUpdateBS = false;
	m_iClassId = 1;
}

CModel* CModel::CreateModel()
{
	CModel* retValue = new CModel();
	s_apAllRoot.push_back(retValue);
	return retValue;
}

bool CModel::IsCModel(const CNode& oNode)
{
	return oNode.GetClassId() == 1;
}

CModel::~CModel()
{
	std::vector<CMaterial::Data>::iterator i;
	for(i=m_aoMaterial.begin();i!=m_aoMaterial.end();++i)
	{
		(*i).Destroy();
	}
}

void CModel::CheckVisibilities(const float4 i_af4Frustum[6])
{
	for(unsigned int i=0;i<s_af4BoundingSpheres.size();i++)
	{
		s_abVisible[i] = true;
		for(unsigned int j=0;j<6;j++)
		{
			// a*cx+b*cy+c*cz+d < radius then -> outside frustum
			if( i_af4Frustum[j][0] * s_af4BoundingSpheres[i][0] + 
				i_af4Frustum[j][1] * s_af4BoundingSpheres[i][1] + 
				i_af4Frustum[j][2] * s_af4BoundingSpheres[i][2] + 
				i_af4Frustum[j][3] < s_af4BoundingSpheres[i][3] )
			{
				s_abVisible[i] = false;
			}
		}
	}
}

void CModel::SortBoundingFunctor::operator()(CNode* p)
{
	CModel *pModel = reinterpret_cast<CModel*>(p);
	if( !pModel->IsKindModel() ) return;
	if( pModel->m_iVisibility != m_iCount )
	{
		pModel->m_iVisibility = m_iCount;
		pModel->SetDirty();
	}
	m_iCount++;
}

void CModel::SortBoundings()
{
	if(!s_bNeedSortMatrices) return;
	SortBoundingFunctor tmp;
	tmp.m_iCount = 0;
	ForAllNodeHierarchy(tmp);
}

void CModel::UpdateBoundingsFunctor::operator()(CNode* p)
{
	CModel *pModel = reinterpret_cast<CModel*>(p);
	if( !pModel->IsKindModel() ) return;
	if( !pModel->IsDirty() &&
		!pModel->m_bNeedUpdateBS ) return;
	pModel->UpdateBounding();
	pModel->m_bNeedUpdateBS = false;
}

void CModel::UpdateBoundings()
{
	UpdateBoundingsFunctor tmp;
	ForAllNodeHierarchy(tmp);
}

void CModel::Draw(bool i_bIgnoreVisibility, bool i_bSetMaterial)
{
	if(!m_bEnabled) return;
	CNode::Draw(i_bIgnoreVisibility,i_bSetMaterial);

	if( !s_abVisible[m_iVisibility] && !i_bIgnoreVisibility ) 
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf( s_af16WorldMatrices[m_iMatrix] );
//	if( i_bSetMaterial )
//		CMaterial::Update();
	for(unsigned int i=0;i<m_apMeshes.size();i++)
	{
		if( i_bSetMaterial )
			CMaterial::SetData( m_aoMaterial[i] );
		m_apMeshes[i]->Draw();
	}
	glPopMatrix();
}

void CModel::ReserveMeshes(unsigned int i_nHowMany)
{
	m_apMeshes.reserve( i_nHowMany );
	m_aoMaterial.reserve( i_nHowMany );
}

unsigned int CModel::AddMesh()
{
	unsigned int id = m_apMeshes.size();
	m_apMeshes.push_back( new CMesh() );
	m_aoMaterial.push_back( CMaterial::Data() );
	m_bNeedUpdateBS = true;
	return id;
}

CMesh* CModel::GetMesh(unsigned int i)
{
	if( i>=m_apMeshes.size() )
	{
		ILog::Error("CModel::GetMesh invalid index %i/%i", i, m_apMeshes.size());
		return 0;
	}
	return m_apMeshes[i];
}

void CModel::SetMaterial(unsigned int i,CMaterial::Data& i_oMaterial)
{
	if(i>=m_aoMaterial.size())
	{
		ILog::Error("CModel::SetMaterial invalid index %i/%i", i, m_aoMaterial.size());
		return;
	}
	m_aoMaterial[i] = i_oMaterial;
}

const CMaterial::Data& CModel::GetMaterial(unsigned int i) const
{
	if(i>=m_aoMaterial.size())
	{
		ILog::Error("CModel::GetMaterial invalid index %i/%i", i, m_aoMaterial.size());
		return CMaterial::Data();
	}
	return m_aoMaterial[i];
}

float CModel::LineCollision(const float3& start,const float3& dir,float len)
{
	float3 localStart(s_af16InvWorldMatrices[m_iMatrix].Transform(float4(start,1)));
	float3 localDir(s_af16InvWorldMatrices[m_iMatrix].Transform(float4(dir,0)));

	float currMin = FLT_MAX;
	std::vector<CMesh*>::iterator i;
	for(i=m_apMeshes.begin();i!=m_apMeshes.end();++i)
	{
		float thisColl = (*i)->LineCollision(localStart,localDir,len);
		if( thisColl < FLT_MAX )
			currMin = thisColl;
	}
	return currMin;
}

void CModel::UpdateBounding()
{
	floatSphere& dest = s_af4BoundingSpheres[m_iVisibility];
	Matrix4f &world = s_af16WorldMatrices[m_iMatrix];
	if( m_apMeshes.empty() )
	{
		dest.Reset();
		return;
	}

	dest = world.TransformSphere(m_apMeshes[0]->GetBoundingSphere());

	std::vector<CMesh*>::iterator i = m_apMeshes.begin()+1;
	for(;i!=m_apMeshes.end();++i)
	{
		dest += world.TransformSphere((*i)->GetBoundingSphere());
	}
	
}

std::vector<floatSphere> CModel::s_af4BoundingSpheres;
std::vector<bool> CModel::s_abVisible;
bool CModel::s_bAddedToUpdates = false;
