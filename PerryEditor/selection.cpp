#include "selection.h"
#include "node.h"
#include "model.h"
#include "opengl.h"

CSelection CSelection::s_pInstance;

CSelection& CSelection::Instance()
{
	return s_pInstance;
}

CSelection::CSelection()
{
}

void CSelection::EvidenceSelection()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glColor4f(1.f,1.f,1.f,1);
	for(iterator i=begin();i!=end();++i)
	{
		(*i)->Draw(true,false);
	}
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void CSelection::GetMatrix(Matrix4f& oDest)
{
	oDest.Zero();
	for(iterator i=begin();i!=end();++i)
	{
		oDest += (*i)->GetWorld();
	}
	oDest /= size();
}

void CSelection::SetMatrix(const Matrix4f& oSrc)
{
	for(iterator i=begin();i!=end();++i)
	{
		(*i)->SetWorld(oSrc);
	}
}

floatSphere CSelection::GetBoundingSphere()
{
	bool oneAdded = false;
	floatSphere bs;
	for(iterator i=begin();i!=end();++i)
	{
		CModel *pModel = reinterpret_cast<CModel*>(*i);
		if( pModel->IsKindModel() )
		{
			if( oneAdded )
			{
				bs += pModel->GetBoundingSphere();
			} else
			{
				bs = pModel->GetBoundingSphere();
				oneAdded = true;
			}
		}
	}
	return bs;
}
