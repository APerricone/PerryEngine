#ifndef SELECTION_H
#define SELECTION_H

#include <QObject>

#include "matrix.h"
#include "sphere.h"
#include <QList>

class CNode;

class CSelection : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Antonino Perricone")

	static CSelection* s_pInstance;
public:
	explicit CSelection(QObject *parent = 0);

signals:
	void selectionChanged(const QList<CNode*>& list);

public:
	static CSelection* Instance();

	void EvidenceSelection();
	void GetMatrix(Matrix4f& oDest);
	void SetMatrix(const Matrix4f& oSrc);
	floatSphere GetBoundingSphere();

	void ClearSelection();
	void SetSelection(CNode*node);
	void AddSelection(CNode*node);
	void DelSelection(CNode*node);


	typedef QList<CNode*> List;
	static const QList<CNode*>& GetList() { return s_pInstance->m_lSelection; }

	void SetSelection(const List &list);
private:
	List m_lSelection;
};

#endif // SELECTION_H
