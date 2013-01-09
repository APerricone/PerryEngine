#ifndef MOUSEMOVE_H
#define MOUSEMOVE_H

#include "mouseaction.h"
#include "matrix.h"
class CMesh;
class QCursor;


class CMouseMove : public IMouseAction
{
public:
	CMouseMove(QObject* pParent);
	~CMouseMove();

	virtual void DrawInsideScene();
	virtual void DrawOverScene();
	virtual void Draw2D();

	virtual void keyPressEvent ( QKeyEvent * event );
	virtual void keyReleaseEvent ( QKeyEvent * event );
	virtual void mousePressEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );
	virtual void mouseMoveEvent( QMouseEvent * event );

private:
	CMesh* m_pArrow;
	CMesh* m_pHalfCross;
	QCursor* m_qMyCursor;

	float2 m_f2AxisLabelPos[3];

	enum eState
	{
		ST_NONE,
		ST_MARKX,
		ST_MARKY,
		ST_MARKZ
	} m_eState;

	float3 m_f3StartingPoint;
	Matrix4f m_f16StartingMatrix;
};

#endif // MOUSEMOVE_H
