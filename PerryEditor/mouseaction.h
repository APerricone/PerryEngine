#ifndef MOUSEACTION_H
#define MOUSEACTION_H

#include "vector.h"
#include <QAction>

class CEditorCamera;
class QMouseEvent;
class QKeyEvent;


class IMouseAction : public QAction
{
protected:
	IMouseAction(QObject* pParent) : QAction(pParent) {}
	~IMouseAction() {}

public:
	void SetCamera(CEditorCamera* pCamera) { m_pCamera=pCamera; }

	virtual void DrawInsideScene() = 0;
	virtual void DrawOverScene() = 0;
	virtual void Draw2D() = 0;

	virtual void keyPressEvent ( QKeyEvent * event ) = 0;
	virtual void keyReleaseEvent ( QKeyEvent * event ) = 0;
	virtual void mousePressEvent( QMouseEvent * event ) = 0;
	virtual void mouseReleaseEvent( QMouseEvent * event ) = 0;
	virtual void mouseMoveEvent( QMouseEvent * event ) = 0;
protected:	
	CEditorCamera *m_pCamera;

	float GetPixelScale(const float3& pos);
};

#endif // MOUSEACTION_H
