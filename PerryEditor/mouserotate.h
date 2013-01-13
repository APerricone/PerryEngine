#ifndef MOUSEROTATE_H
#define MOUSEROTATE_H

#include "mouseaction.h"
#include "matrix.h"
class CMesh;
class QCursor;

class CMouseRotate : public IMouseAction
{
public:
	CMouseRotate(QObject* pParent);
	~CMouseRotate();

	virtual void DrawInsideScene();
	virtual void DrawOverScene();
	virtual void Draw2D();

	virtual void keyPressEvent ( QKeyEvent * event );
	virtual void keyReleaseEvent ( QKeyEvent * event );
	virtual void mousePressEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );
	virtual void mouseMoveEvent( QMouseEvent * event );

private:
	CMesh* m_pCircle;
	QCursor* m_qMyCursor;

	enum eState
	{
		ST_NONE,
		ST_MARKX,
		ST_MARKY,
		ST_MARKZ
	} m_eState;

	float3 m_f3StartingPoint;
	float3 m_f3StartingDir;
	float3 m_f3StartingOrtho;
	Matrix4f m_f16StartingMatrix;
};


#endif // MOUSEROTATE_H
