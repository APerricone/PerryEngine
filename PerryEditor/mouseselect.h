#ifndef MOUSESELECT_H
#define MOUSESELECT_H

#include "vector.h"
#include "mouseaction.h"
class CMesh;
class QCursor;

class CMouseSelect : public IMouseAction
{
public:
	CMouseSelect(QObject* pParent);
	~CMouseSelect();

	virtual void DrawInsideScene();
	virtual void DrawOverScene();
	virtual void Draw2D();

	virtual void keyPressEvent ( QKeyEvent * event );
	virtual void keyReleaseEvent ( QKeyEvent * event );
	virtual void mousePressEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );
	virtual void mouseMoveEvent( QMouseEvent * event );

private:
	CMesh* m_pAxe;
	QCursor* m_qAddCursor;
	QCursor* m_qRemCursor;

	float4 m_f4CurrentPos;
	float4 m_f4CurrentRight;
	float4 m_f4CurrentUp;
	float4 m_f4CurrentAt;

	void Draw(float fAlpha);
};

#endif // MOUSESELECT_H
