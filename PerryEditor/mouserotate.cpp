#include "mouserotate.h"
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
#include <QMouseEvent>

CMouseRotate::CMouseRotate(QObject* pParent) :
	IMouseAction(pParent)
{
	m_eState = ST_NONE;
	setObjectName(QString::fromUtf8("mouse_rotate"));
	setCheckable(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/edit/icons/rotate_icon.png"), QSize(), QIcon::Normal, QIcon::On);
	setIcon(icon);
	setText(QApplication::translate("MouseActions", "&Move", 0, QApplication::UnicodeUTF8));
	setShortcut(QApplication::translate("MouseActions", "R", 0, QApplication::UnicodeUTF8));

	CDynamicMesh oCreator;
	// 100 pixel len = 1 unit
	// 3 pixel width = 0.03 unit
	oCreator.AddTorus(float3(0.f,0.f,0.f),1.f,0.015f,8,32);
	m_pCircle = new CMesh();
	oCreator.CreateMesh(*m_pCircle);

	m_qMyCursor = new QCursor(QPixmap(":/select/cursors/arrow_rotate.png"), 2, 2);
}

CMouseRotate::~CMouseRotate()
{
	delete m_pCircle;
	delete m_qMyCursor;
}

void CMouseRotate::DrawInsideScene()
{

}

void CMouseRotate::DrawOverScene()
{
	if( CSelection::Instance().size()==0 ) return;
	Matrix4f mWorld;
	CSelection::Instance().GetMatrix(mWorld);
	float s = GetPixelScale( float3(mWorld.GetPos()) );
	float4 p = mWorld.GetPos(); p[3] = 1.f;
	float4 r = mWorld.GetRg()*s;
	float4 u = mWorld.GetUp()*s;
	float4 a = mWorld.GetAt()*s;

	Matrix4f tmp;
	tmp.SetPos(p);

	m_pCircle->PreDraw();

	glPushMatrix();
	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKX)
		glColor4f(1,0,0,1.0f);
	else
		glColor4f(1,1,1,1.0f);
	m_pCircle->ReDraw();
	glPopMatrix();

	glPushMatrix();
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKY)
		glColor4f(0,1,0,1.0f);
	else
		glColor4f(1,1,1,1.0f);
	m_pCircle->ReDraw();
	glPopMatrix();

	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	tmp.SetPos(p);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKZ)
		glColor4f(0,0,1,1.0f);
	else
		glColor4f(1,1,1,1.0f);
	m_pCircle->ReDraw();
	glPopMatrix();

	m_pCircle->EndDraw();
}

void CMouseRotate::Draw2D()
{
}

void CMouseRotate::keyPressEvent ( QKeyEvent * event )
{
	CMouseActions::GetDefault()->keyPressEvent(event);
}

void CMouseRotate::keyReleaseEvent ( QKeyEvent * event )
{
	CMouseActions::GetDefault()->keyReleaseEvent(event);
}

void CMouseRotate::mousePressEvent( QMouseEvent * event )
{
	if( m_eState == ST_NONE)
	{
		CMouseActions::GetDefault()->mousePressEvent(event);
	} else
	{
		CSelection::Instance().GetMatrix(m_f16StartingMatrix);
	}
}

void CMouseRotate::mouseReleaseEvent( QMouseEvent * event )
{
	if( m_eState == ST_NONE)
	{
		CMouseActions::GetDefault()->mouseReleaseEvent(event);
	} else
	{
	}
}

void CMouseRotate::mouseMoveEvent( QMouseEvent * event )
{
	if( CSelection::Instance().size()==0 ) return;
	Matrix4f mWorld;
	CSelection::Instance().GetMatrix(mWorld);
/*	float s = GetPixelScale( float3(mWorld.GetPos()) );
	float3 p = float4(mWorld.GetPos()).xyz();
	float3 r = float4(mWorld.GetRg()).xyz()*s;
	float3 u = float4(mWorld.GetUp()).xyz()*s;
	float3 a = float4(mWorld.GetAt()).xyz()*s;
	float3 pru = p+(r+u)*0.8f;
	float3 pra = p+(r+a)*0.8f;
	float3 pua = p+(u+a)*0.8f;

	Ray3f camRay(m_pCamera->Get3DRay(float3(event->x(),event->y(),0.f)));

	if( event->buttons() != Qt::NoButton )
	{
		float t,rr;
		float3 newP;
		float3 movement;
		switch(m_eState)
		{
		case ST_MARKX:
			camRay.RayDistance(Ray3f(p,r),t,rr);
			newP = p+r*rr;
			break;
		case ST_MARKY:
			camRay.RayDistance(Ray3f(p,u),t,rr);
			newP = p+u*rr;
			break;
		case ST_MARKZ:
			camRay.RayDistance(Ray3f(p,a),t,rr);
			newP = p+a*rr;
			break;
		case ST_MARKXY:
			camRay.PlaneIntersection(pru,a,t);
			newP = camRay.GetPoint(t);
			break;
		case ST_MARKXZ:
			camRay.PlaneIntersection(pra,u,t);
			newP = camRay.GetPoint(t);
			break;
		case ST_MARKYZ:
			camRay.PlaneIntersection(pua,r,t);
			newP = camRay.GetPoint(t);
			break;
		default:
			return;
		}
		movement = newP - m_f3StartingPoint;
		switch(m_eState)
		{
		case ST_MARKX:
			movement = r * (dot(movement,r) / (s*s));
			break;
		case ST_MARKY:
			movement = u * (dot(movement,u) / (s*s));
			break;
		case ST_MARKZ:
			movement = a * (dot(movement,a) / (s*s));
			break;
		case ST_MARKXY:
			movement -= a * (dot(movement,a) / (s*s));
			break;
		case ST_MARKXZ:
			movement -= u * (dot(movement,u) / (s*s));
			break;
		case ST_MARKYZ:
			movement -= r * (dot(movement,r) / (s*s));
			break;
		default:
			return;
		}
		mWorld = m_f16StartingMatrix;
		mWorld.Translate( movement );
		CSelection::Instance().SetMatrix(mWorld);
	} else
	{
		float t;
		float rx,dx = camRay.SegmentDistance(Ray3f(p,r),t,rx);
		float ry,dy = camRay.SegmentDistance(Ray3f(p,u),t,ry);
		float rz,dz = camRay.SegmentDistance(Ray3f(p,a),t,rz);
		float rxy,dxy = camRay.PointDistance(pru,rxy);
		float rxz,dxz = camRay.PointDistance(pra,rxz);
		float ryz,dyz = camRay.PointDistance(pua,ryz);
		float d = 1/s;
		m_eState = ST_NONE;
		if( dx < d )
		{
			m_eState = ST_MARKX;
			d = dx;
			m_f3StartingPoint = p + r * rx ;
		}
		if( dy < d )
		{
			m_eState = ST_MARKY;
			d = dy;
			m_f3StartingPoint = p + u * ry ;
		}
		if( dz < d )
		{
			m_eState = ST_MARKZ;
			d = dz;
			m_f3StartingPoint = p + a * rz ;
		}
		if( dxy < d )
		{
			m_eState = ST_MARKXY;
			d = dxy;
			m_f3StartingPoint = camRay.GetPoint(rxy) ;
		}
		if( dxz < d )
		{
			m_eState = ST_MARKXZ;
			d = dxz;
			m_f3StartingPoint = camRay.GetPoint(rxz) ;
		}
		if( dyz < d )
		{
			m_eState = ST_MARKYZ;
			d = dyz;
			m_f3StartingPoint = camRay.GetPoint(ryz) ;
		}
	}*/
}

