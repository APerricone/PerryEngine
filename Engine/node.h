#pragma once

#include <list>
#include <vector>
#include "matrix.h"
#include "float.h"

// m_iClassId table:
// 0 Node
// 1 Model - all odd is model
// TODO:
// 2 Light
// 3 Skinned
// 

class CNode
{
protected:
	CNode();

public:
	static CNode* CreateNode();
	virtual ~CNode();

	void SetName(const char* name);
	const char* GetName() const { return m_strName; }
	static CNode* GetByName(const char* name);

	static void GlobalUpdate();

	void SetParent(CNode*);
	void AddChild(CNode*);

	CNode* GetParent() { return m_pParent; }
	const CNode* GetParent() const { return m_pParent; }

	void SetDirty() { s_abDirty[m_iMatrix] = true; }
	bool IsDirty() const { return s_abDirty[m_iMatrix]; }

	void SetLocal(const Matrix4f& m) { s_af16LocalMatrices[m_iMatrix] = m; s_abDirty[m_iMatrix] = true; }
	const Matrix4f& GetLocal() const { return s_af16LocalMatrices[m_iMatrix]; }
	const Matrix4f& GetWorld() const { return s_af16WorldMatrices[m_iMatrix]; }
	const Matrix4f& GetInvWorld() const { return s_af16InvWorldMatrices[m_iMatrix]; }

	static void DrawAll(bool i_bIgnoreVisibility, bool i_bSetMaterial);
	virtual void Draw(bool i_bIgnoreVisibility, bool i_bSetMaterial);

	virtual float LineCollision(const float3& start,const float3& dir,float len = FLT_MAX) { return FLT_MAX; }

	template<typename fn>
	static void ForAllNodeHierarchy(fn& i_tFn);
	template<typename fn>
	void ForAllChildrenHierarchy(fn& i_tFn);
protected:
	char *m_strName;

	unsigned int m_iClassId;

	//! parent node
	//! \note If null this node is inside #s_apAllRoot
	CNode* m_pParent;
	//! 
	typedef std::list<CNode*> ChildrenContainer;
	//! Children nodes
	ChildrenContainer m_apChildren;
	//! index of matrices and parentId
	//! \sa s_af16LocalMatrices, s_af16WorldMatrices, 
	//! \sa s_af16InvWorldMatrices, s_aiParentId
	unsigned int m_iMatrix;
	
	void RemoveFromList(ChildrenContainer& list);
	unsigned int SortIt(unsigned int id);
	void SaveParentId(unsigned int id);


	//! list of all root nodes
	static ChildrenContainer s_apAllRoot;
	//! All local matrices.
	//! \note Cache friendly
	static std::vector<Matrix4f> s_af16LocalMatrices;
	//! All world matrices.
	//! \note Cache friendly
	static std::vector<Matrix4f> s_af16WorldMatrices;
	//! All inverse world matrices.
	//! \note Cache friendly
	static std::vector<Matrix4f> s_af16InvWorldMatrices;
	//! Indices of parent matrices.
	//! \note Cache friendly
	static std::vector<unsigned int> s_aiParentId;
	//! Dirty flags
	//! \note Cache friendly
	static std::vector<bool> s_abDirty;
	//! A complete re-sort of matrices is needed when parent relations
	//! are changed or a node is deleted.
	static bool s_bNeedSortMatrices;


	static void SortMatrices();
	static void UpdateMatrices();

	typedef void (*NoParamNoReturn)(void);
	static std::vector<NoParamNoReturn> s_afUpdatePasses;


	//! Functor for drawing
	struct DrawFunctor
	{
		DrawFunctor(bool i_bIgnoreVisibility, bool i_bSetMaterial) : 
			m_bIgnoreVisibility(i_bIgnoreVisibility), 
			m_bSetMaterial(i_bSetMaterial)
			{ }

		void operator()(CNode*);
		bool m_bIgnoreVisibility;
		bool m_bSetMaterial;
	};
};

/// INL
template<typename fn>
inline void CNode::ForAllChildrenHierarchy(fn& i_tFn)
{
	ChildrenContainer::iterator iNode;
	for(iNode=m_apChildren.begin();iNode!=m_apChildren.end();iNode++)
	{
		i_tFn(*iNode);
		if( (*iNode)->m_apChildren.size() != 0)
			(*iNode)->ForAllChildrenHierarchy(i_tFn);
	}
}

template<typename fn>
inline void CNode::ForAllNodeHierarchy(fn& i_tFn)
{
	ChildrenContainer::iterator iNode;
	for(iNode=s_apAllRoot.begin();iNode!=s_apAllRoot.end();iNode++)
	{
		i_tFn(*iNode);
		if( (*iNode)->m_apChildren.size() != 0)
			(*iNode)->ForAllChildrenHierarchy(i_tFn);
	}
}
