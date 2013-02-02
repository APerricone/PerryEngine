#include "node.h"
#include "mymacros.h"
#include <algorithm>

#include <string.h>

CNode::CNode() : m_pParent(NULL), m_strName(NULL)
{
	if(s_afUpdatePasses.size()== 0)
	{ //init statics
		s_afUpdatePasses.push_back(SortMatrices);
		s_afUpdatePasses.push_back(UpdateMatrices);
	}
	m_iMatrix = s_af16LocalMatrices.size();

	s_af16LocalMatrices.push_back(Matrix4f());
	s_af16LocalMatrices.back().Identity();
	s_af16WorldMatrices.push_back(Matrix4f());
	s_af16InvWorldMatrices.push_back(Matrix4f());
	s_aiParentId.push_back(~0);
	s_abDirty.push_back(true);
	m_iClassId = 0;
	m_bEnabled = true;
}

CNode* CNode::CreateNode()
{
	CNode* retValue = new CNode();
	s_apAllRoot.push_back(retValue);
	return retValue;
}

void CNode::SetName(const char* name)
{
	SAFE_DELETE_ARRAY(m_strName);
	int l = strlen(name);
	m_strName = new char[l+1];
	strcpy(m_strName,name);
}

CNode::~CNode()
{
	SAFE_DELETE_ARRAY(m_strName);
	SetParent(NULL);
	ChildrenContainer::iterator i;
	for(i=m_apChildren.begin();i!=m_apChildren.end();++i)
	{
		delete (*i);
	}
}

void CNode::SetParent(CNode* pParent)
{
	if( pParent == m_pParent ) 
		return;

	if( m_pParent == NULL )
	{
		RemoveFromList( s_apAllRoot );
	} else
	{
		RemoveFromList( m_pParent->m_apChildren );
	}

	if( pParent == NULL) 
	{
		s_apAllRoot.push_back( this );
	} else
	{
		pParent->m_apChildren.push_back( this );
	}

	m_pParent = pParent;
	s_bNeedSortMatrices = true;
}

void CNode::AddChild(CNode* pChild)
{
	pChild->SetParent( this );
}

void CNode::RemoveFromList(ChildrenContainer& list)
{
	ChildrenContainer::iterator i =
		std::find(list.begin(),list.end(), this );
	//if( i == list.end() ) ILog::Error("error");
	list.erase(i);
}

void CNode::SortMatrices()
{
	if(!s_bNeedSortMatrices) return;
	// this method is a optimized for memory, It does not create new data, but 
	// use irregulary some think:
	//  * s_aiParentId saves the old Ids
	//  * s_af16WorldMatrices saves LocalMatrices
	
	// First: copy all local into world
	unsigned int Id;
	for(Id=0;Id<s_af16LocalMatrices.size();Id++)
	{
		s_af16WorldMatrices[Id] = s_af16LocalMatrices[Id];
	}
	// Second, assign a new Id to all nodes, 
	// saving in s_aiParentId the old Ids, u
	Id = 0;
	ChildrenContainer::iterator iNode;
	for(iNode=s_apAllRoot.begin();iNode!=s_apAllRoot.end();++iNode)
	{
		Id = (*iNode)->SortIt(Id);
	}
	// Thirst copy old local int new local position
	for(Id=0;Id<s_af16LocalMatrices.size();Id++)
	{
		s_af16LocalMatrices[Id] = s_af16WorldMatrices[s_aiParentId[Id]];
	}
	// Set all dirty... It is heavy!!
	for(Id=0;Id<s_af16LocalMatrices.size();Id++)
	{
		s_abDirty[Id] = true;
	}
	// Fill parentIds with right values
	for(iNode=s_apAllRoot.begin();iNode!=s_apAllRoot.end();++iNode)
	{
		(*iNode)->SaveParentId(~0);
	}
}

unsigned int CNode::SortIt(unsigned int id)
{
	s_aiParentId[id] = m_iMatrix;
	m_iMatrix = id;
	id++;

	ChildrenContainer::iterator iNode;
	for(iNode=m_apChildren.begin();iNode!=m_apChildren.end();++iNode)
	{
		id = (*iNode)->SortIt(id);
	}
	return id;
}

void CNode::SaveParentId(unsigned int id)
{
	s_aiParentId[m_iMatrix] = id;

	ChildrenContainer::iterator iNode;
	for(iNode=m_apChildren.begin();iNode!=m_apChildren.end();++iNode)
	{
		(*iNode)->SaveParentId(m_iMatrix);
	}
}

void CNode::SetWorld(const Matrix4f& m)
{
	if( m_pParent )
	{
		Matrix4f mm;
		mm.Multiply(m_pParent->GetInvWorld(),m);
		SetLocal( mm );
	} else
		SetLocal( m );
}

void CNode::UpdateMatrices()
{
	// calculate worlds
	unsigned int n = s_af16LocalMatrices.size();
	for(unsigned int i=0;i<n;i++)
	{
		unsigned int p = s_aiParentId[i];
		if( p == ~0 )
		{
			s_af16WorldMatrices[i] = s_af16LocalMatrices[i];
		}
		else
		{
			if(s_abDirty[p]) s_abDirty[i] = true;
			if(s_abDirty[i])
				s_af16WorldMatrices[i].Multiply(
							s_af16WorldMatrices[p],
							s_af16LocalMatrices[i]);
		}
	}
	// calculate inverse
	for(unsigned int i=0;i<n;i++)
	{
		if(s_abDirty[i])
			s_af16WorldMatrices[i].GetInvert(s_af16InvWorldMatrices[i]);
	}
}

void CNode::DrawFunctor::operator()(CNode* p)
{
	p->Draw(m_bIgnoreVisibility,m_bSetMaterial);
}

void CNode::Draw(bool i_bIgnoreVisibility, bool i_bSetMaterial)
{
}

void CNode::DrawAll(bool i_bIgnoreVisibility, bool i_bSetMaterial)
{
	DrawFunctor tmp(i_bIgnoreVisibility,i_bSetMaterial);
	ForAllNodeHierarchy(tmp);
}

struct SearchByName
{
	SearchByName(const char* _name) : name(_name) { result = 0; }
	CNode *result;
	const char* name;
	void operator()(CNode* p)
	{
		if( p->GetName() )
			if( strcmp(p->GetName(),name) == 0 )
				result = p;
	}
};

CNode* CNode::GetByName(const char* name)
{
	SearchByName tmp(name);
	ForAllNodeHierarchy(tmp);
	return tmp.result;
}

void CNode::GlobalUpdate()
{
	std::vector<CNode::NoParamNoReturn>::iterator i;
	for(i=s_afUpdatePasses.begin();i!=s_afUpdatePasses.end();++i)
	{
		(*i)();
	}
	s_bNeedSortMatrices = false;
	for(unsigned int i=0;i<s_abDirty.size();i++)
		s_abDirty[i] = false;
}

CNode::ChildrenContainer CNode::s_apAllRoot;
std::vector<Matrix4f> CNode::s_af16LocalMatrices;
std::vector<Matrix4f> CNode::s_af16WorldMatrices;
std::vector<Matrix4f> CNode::s_af16InvWorldMatrices;
std::vector<unsigned int> CNode::s_aiParentId;
std::vector<bool> CNode::s_abDirty;
std::vector<CNode::NoParamNoReturn> CNode::s_afUpdatePasses;
bool CNode::s_bNeedSortMatrices = false;
