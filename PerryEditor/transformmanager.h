#ifndef TRANSFORMMANAGER_H
#define TRANSFORMMANAGER_H

#include "vector.h"
class CMesh;
class CNode;
class QMouseEvent;
class CEditorCamera;

class CTransformManager
{
public:
    CTransformManager();
	~CTransformManager();

	enum eMode
	{
		eSELECTION,
		eMOVE,
		eROTATE,
		eSCALE
	};

	void SetSelection(CNode* pSelection) { m_pSelection=pSelection; }
	void SetCamera(CEditorCamera* pCamera) { m_pCamera=pCamera; }
	void SetMode(eMode mode) { m_eMode = mode; }
	void Draw();
	void Draw2D();

	void mousePressEvent( QMouseEvent * event );
	void mouseReleaseEvent( QMouseEvent * event );
	void mouseMoveEvent( QMouseEvent * event );

private:
	CEditorCamera *m_pCamera;
	CMesh *m_pMoveArrow;
	CMesh *m_pMoveHalfCross;

	float2 m_af2TextPositions[4];
	/*
	enum eGizmoState
	{
		eNone = 0,
		eX = 1,
		eY = 2,
		eZ = 4,
		eXY = 3,
		eXZ = 5,
		eYZ = 6,
		eXYZ = 7,
		eTOTALSTATES = 8
	};

	CDynamicMesh *m_pMoveGizmo[eTOTALSTATES];
	CDynamicMesh *m_pRotateGizmo[eTOTALSTATES];
	CDynamicMesh *m_pScaleGizmo[eTOTALSTATES];*/
	CNode* m_pSelection;
	eMode m_eMode;
	//eGizmoState m_eState;
};

#endif // TRANSFORMMANAGER_H
