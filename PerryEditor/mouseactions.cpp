#include "mouseactions.h"
#include <QMenu>

CMouseActions* CMouseActions::s_pInstance = NULL;

void CMouseActions::Init(QObject* pParent,QMenu* pMenu)
{
	Q_ASSERT_X(s_pInstance==NULL,"CMouseActions::Init","Already initialized");
	s_pInstance = new CMouseActions(pParent);
	s_pInstance->m_pMenu = pMenu;
}

CMouseActions::CMouseActions(QObject* pParent) :
	QActionGroup(pParent)
{
	setObjectName(QString::fromUtf8("mouse_actions"));
}

void CMouseActions::AddAction(IMouseAction* pMouse)
{
	s_pInstance->QActionGroup::addAction(pMouse);
	s_pInstance->m_pMenu->addAction(pMouse);
	pMouse->SetCamera(s_pInstance->m_pCamera);
}

void CMouseActions::RemoveAction(IMouseAction* pMouse)
{
	s_pInstance->QActionGroup::removeAction(pMouse);
	s_pInstance->m_pMenu->removeAction(pMouse);
}

inline IMouseAction* CMouseActions::CurrentAction()
{
	return dynamic_cast<IMouseAction*>(checkedAction());
}

void CMouseActions::DrawInsideScene()
{
	s_pInstance->CurrentAction()->DrawInsideScene();
}

void CMouseActions::DrawOverScene()
{
	s_pInstance->CurrentAction()->DrawOverScene();
}

void CMouseActions::Draw2D()
{
	s_pInstance->CurrentAction()->Draw2D();
}

void CMouseActions::keyPressEvent( QKeyEvent * event )
{
	s_pInstance->CurrentAction()->keyPressEvent(event);
}

void CMouseActions::keyReleaseEvent( QKeyEvent * event )
{
	s_pInstance->CurrentAction()->keyReleaseEvent(event);
}

void CMouseActions::mousePressEvent( QMouseEvent * event )
{
	s_pInstance->CurrentAction()->mousePressEvent(event);
}

void CMouseActions::mouseReleaseEvent( QMouseEvent * event )
{
	s_pInstance->CurrentAction()->mouseReleaseEvent(event);
}

void CMouseActions::mouseMoveEvent( QMouseEvent * event )
{
	s_pInstance->CurrentAction()->mouseMoveEvent(event);
}
