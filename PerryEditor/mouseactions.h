#ifndef MOUSEACTIONS_H
#define MOUSEACTIONS_H

#include <QActionGroup>
#include "mouseaction.h"
#include <QMap>

class CMouseActions : public QActionGroup
{
	CMouseActions(QObject* pParent);
	static CMouseActions* s_pInstance;
public:
	static void Init(QObject* pParent,QMenu* pMenu);

	static void SetCamera(CEditorCamera* pCamera) { s_pInstance->m_pCamera=pCamera; }

	static void AddAction(IMouseAction* pMouse);
	static void RemoveAction(IMouseAction* pMouse);
	static void SetDefault(IMouseAction* pMouse) { s_pInstance->m_pDefaultAction = pMouse; }
	static IMouseAction* GetDefault() { return s_pInstance->m_pDefaultAction; }

	static void DrawInsideScene();
	static void DrawOverScene();
	static void Draw2D();
	static void keyPressEvent( QKeyEvent * event );
	static void keyReleaseEvent( QKeyEvent * event );
	static void mousePressEvent( QMouseEvent * event );
	static void mouseReleaseEvent( QMouseEvent * event );
	static void mouseMoveEvent( QMouseEvent * event );
private:
	CEditorCamera *m_pCamera;
	QMenu* m_pMenu;

	IMouseAction* m_pDefaultAction;

	IMouseAction* CurrentAction();
};

#endif // MOUSEACTIONS_H
