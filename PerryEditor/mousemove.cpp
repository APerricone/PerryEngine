#include "mousemove.h"
// from PerryEditor
#include "selection.h"
#include "mouseactions.h"
#include "editorcamera.h"
// from Engine
#include "mesh.h"
#include "dynamicmesh.h"
#include "matrix.h"
#include "opengl.h"
#include "font.h"
// from Qt
#include <QCursor>
#include <QApplication>

#include "capsule.h"

CMouseMove::CMouseMove(QObject* pParent) :
	IMouseAction(pParent)
{
	setObjectName(QString::fromUtf8("mouse_move"));
	setCheckable(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/edit/icons/move_icon.png"), QSize(), QIcon::Normal, QIcon::On);
	setIcon(icon);
	setText(QApplication::translate("MouseActions", "&Move", 0, QApplication::UnicodeUTF8));
	setShortcut(QApplication::translate("MouseActions", "E", 0, QApplication::UnicodeUTF8));

	CDynamicMesh oCreator;
	// 100 pixel len
	// 3 pixel width
	oCreator.AddCone(float3(0.f,0.f,0.f),0.015f,float3(0.f,0.f,0.8f),0.015f,16,false,false);
	oCreator.AddCone(float3(0.f,0.f,0.8f),0.1f,float3(0.f,0.f,1.f),0.0f,16,true,false);
	m_pArrow = new CMesh();
	oCreator.CreateMesh(*m_pArrow);

	oCreator.Clear();
	oCreator.AddCone(float3(0.f,0.f,-0.1f),0.015f,float3(0.f,0.f,0.1f),0.015f,16,false,false);
	oCreator.AddCone(float3(0.f,0.f,-0.1f),0.05f,float3(0.f,0.f,-0.2f),0.0f,16,true,false);
	oCreator.AddCone(float3(0.f,0.f,0.2f),0.0f,float3(0.f,0.f,0.1f),0.05f,16,false,true);
	m_pHalfCross = new CMesh();
	oCreator.CreateMesh(*m_pHalfCross);

	m_qMyCursor = new QCursor(QPixmap(":/select/cursors/arrow_move.png"), 2, 2);
}

CMouseMove::~CMouseMove()
{
	delete m_pArrow;
	delete m_pHalfCross;
	delete m_qMyCursor;
}
void CMouseMove::DrawInsideScene()
{

}

void CMouseMove::DrawOverScene()
{
	if( CSelection::Instance().size()==0 ) return;
	Matrix4f mWorld;
	CSelection::Instance().GetMatrix(mWorld);
	float s = GetPixelScale( float3(mWorld.GetPos()) );
	float4 p = mWorld.GetPos(); p[3] = 1.f;
	float4 r = mWorld.GetRg()*s;
	float4 u = mWorld.GetUp()*s;
	float4 a = mWorld.GetAt()*s;

	m_f2AxisLabelPos[0] = float2( m_pCamera->Get2DPixel( float3(p + r*1.1f) ) );
	m_f2AxisLabelPos[1] = float2( m_pCamera->Get2DPixel( float3(p + u*1.1f) ) );
	m_f2AxisLabelPos[2] = float2( m_pCamera->Get2DPixel( float3(p + a*1.1f) ) );

	Matrix4f tmp;
	tmp.SetPos(p);

	m_pArrow->PreDraw();

	glPushMatrix();
	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
	glMultMatrixf(tmp);
	glColor4f(1,0,0,1.0f);
	m_pArrow->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glMultMatrixf(tmp);
	glColor4f(0,1,0,1.0f);
	m_pArrow->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	tmp.SetPos(p);
	glMultMatrixf(tmp);
	glColor4f(0,0,1,1.0f);
	m_pArrow->ReDraw();
	glPopMatrix();
	m_pArrow->EndDraw();

	m_pHalfCross->PreDraw();

	glColor4f(1,0,0,1.0f);
	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
	glPushMatrix();
	tmp.SetPos(p + (r+u) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetPos(p + (r+a) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();

	glColor4f(0,1,0,1.0f);
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glPushMatrix();
	tmp.SetPos(p + (r+u) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetPos(p + (u+a) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();

	glColor4f(0,0,1,1.0f);
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	glPushMatrix();
	tmp.SetPos(p + (r+a) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetPos(p + (u+a) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();

	m_pHalfCross->EndDraw();
}

void CMouseMove::Draw2D()
{
	if( CSelection::Instance().size()==0 ) return;
	glColor4f(1,0,0,1.0f);
	glPrint(m_f2AxisLabelPos[0][0]-5,m_f2AxisLabelPos[0][1]-5,"X");
	glColor4f(0,1,0,1.0f);
	glPrint(m_f2AxisLabelPos[1][0]-5,m_f2AxisLabelPos[1][1]-5,"Y");
	glColor4f(0,0,1,1.0f);
	glPrint(m_f2AxisLabelPos[2][0]-5,m_f2AxisLabelPos[2][1]-5,"Z");
}

void CMouseMove::keyPressEvent ( QKeyEvent * event )
{
	CMouseActions::GetDefault()->keyPressEvent(event);
}

void CMouseMove::keyReleaseEvent ( QKeyEvent * event )
{
	CMouseActions::GetDefault()->keyReleaseEvent(event);
}

void CMouseMove::mousePressEvent( QMouseEvent * event )
{
	CMouseActions::GetDefault()->mousePressEvent(event);
}

void CMouseMove::mouseReleaseEvent( QMouseEvent * event )
{
	CMouseActions::GetDefault()->mouseReleaseEvent(event);
}

void CMouseMove::mouseMoveEvent( QMouseEvent * event )
{
	CMouseActions::GetDefault()->mouseMoveEvent(event);
}
