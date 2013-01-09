#ifndef SELECTION_H
#define SELECTION_H

#include "matrix.h"
#include "sphere.h"
#include <QList>
class CNode;

class CSelection : public QList<CNode*>
{
	CSelection();
	static CSelection s_pInstance;
public:
	static CSelection& Instance();

	void EvidenceSelection();
	void GetMatrix(Matrix4f& oDest);
	void SetMatrix(const Matrix4f& oSrc);
	floatSphere GetBoundingSphere();
};

#endif // SELECTION_H
