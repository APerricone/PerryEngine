#include "selection.h"
#include "node.h"
#include "model.h"
#include "opengl.h"

#include <QApplication>

CSelection* CSelection::s_pInstance;

CSelection *CSelection::Instance()
{
	if( s_pInstance==NULL )
		s_pInstance = new CSelection(0);
	return s_pInstance;
}

CSelection::CSelection(QObject *parent) : QObject(parent)
{
}

void CSelection::EvidenceSelection()
{
	if(m_lSelection.empty()) return;
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glColor4f(1.f,1.f,1.f,1);
	for(List::iterator i=m_lSelection.begin();i!=m_lSelection.end();++i)
	{
		(*i)->Draw(true,false);
	}
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void CSelection::GetMatrix(Matrix4f& oDest)
{
	if(m_lSelection.empty())
	{
		oDest.Identity();
		return;
	}
	oDest.Zero();
	for(List::iterator i=m_lSelection.begin();i!=m_lSelection.end();++i)
	{
		oDest += (*i)->GetWorld();
	}
	oDest /= m_lSelection.size();
}

void CSelection::SetMatrix(const Matrix4f& oSrc)
{
	for(List::iterator i=m_lSelection.begin();i!=m_lSelection.end();++i)
	{
		(*i)->SetWorld(oSrc);
	}
}

floatSphere CSelection::GetBoundingSphere()
{
	bool oneAdded = false;
	floatSphere bs;
	for(List::iterator i=m_lSelection.begin();i!=m_lSelection.end();++i)
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

void CSelection::ClearSelection()
{
	m_lSelection.clear();
	emit selectionChanged(m_lSelection);
}

void CSelection::SetSelection(CNode* node)
{
	m_lSelection.clear();
	m_lSelection.append(node);
	emit selectionChanged(m_lSelection);
}

void CSelection::SetSelection(const List& list)
{
	m_lSelection = list;
	emit selectionChanged(m_lSelection);
}

void CSelection::AddSelection(CNode* node)
{
	m_lSelection.append(node);
	emit selectionChanged(m_lSelection);
}

void CSelection::DelSelection(CNode* node)
{
	m_lSelection.removeOne(node);
	emit selectionChanged(m_lSelection);
}

