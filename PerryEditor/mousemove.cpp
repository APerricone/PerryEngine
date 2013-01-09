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
#include <QMouseEvent>

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
	oCreator.AddCone(float3(0.f,0.f,0.8f),0.045f,float3(0.f,0.f,1.f),0.0f,16,true,false);
	m_pArrow = new CMesh();
	oCreator.CreateMesh(*m_pArrow);

	oCreator.Clear();
	oCreator.AddCone(float3(0.f,0.f,-0.1f),0.007f,float3(0.f,0.f,0.1f),0.007f,16,false,false);
	oCreator.AddCone(float3(0.f,0.f,-0.1f),0.021f,float3(0.f,0.f,-0.2f),0.0f,16,true,false);
	oCreator.AddCone(float3(0.f,0.f,0.2f),0.0f,float3(0.f,0.f,0.1f),0.021f,16,false,true);
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
	if( m_eState != ST_MARKX)
		glColor4f(0.75f,0.125f,0.125f,1.0f);
	else
		glColor4f(1,1,1,1.0f);
	m_pArrow->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKY)
		glColor4f(0.125f,0.75f,0.125f,1.0f);
	else
		glColor4f(1,1,1,1.0f);
	m_pArrow->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	tmp.SetPos(p);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKZ)
		glColor4f(0.125f,0.125f,0.75f,1.0f);
	else
		glColor4f(1,1,1,1.0f);
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
	if( m_eState == ST_NONE)
	{
		CMouseActions::GetDefault()->mousePressEvent(event);
	} else
	{
		CSelection::Instance().GetMatrix(m_f16StartingMatrix);
	}
}

void CMouseMove::mouseReleaseEvent( QMouseEvent * event )
{
	CMouseActions::GetDefault()->mouseReleaseEvent(event);
}

float RayPointDistance(
		const float3 &s,const float3 &d,
		const float3 &a)
{
	// P = s + d*t
	// find P1 -> a.d = P1.d && P1 = s + d*t
	// a.d = (s+d*t).d = s.d + d.d*t
	// t = a.d - s.d / d.d
	float t = dot((a-s),d) / dot(d,d);
	float3 P1 = s + d * t;
	return (a-P1).GetLen();
}

float RayRayDistance(
		const float3 &s,const float3 &d,float& t,
		const float3 &a,const float3 &b,float& r)
{
	// P = s + d*t
	// Q = a + b*r
	// find P1 and Q1 -> (P1-Q1).d == 0 && (P1-Q1).b == 0
	// P1-Q1 = s + d*t - (a + b*r) = (s-a) + d*t - b*r
	// (P1-Q1).d = (s-a).d + d.d*t - b.d*r
	// (P1-Q1).b = (s-a).b + d.b*t - b.b*r
	// l = -(s-a).d --- m = d.d --- n =-b.d
	// o = -(s-a).b --- p = d.b --- q =-b.b
	float3 sma = s-a;
	float l =-dot(sma,d);
	float m = dot(d,d);
	float n =-dot(b,d);
	float o =-dot(sma,b);
	float p = dot(d,b);
	float q =-dot(b,b);
	// m*t + n*r = l
	// p*t + q*r = o
	float det = m * q - p * n;
	float dett = l * q - o * n;
	float detr = m * o - p * l;
	if( det == 0 )
	{// parallel
		if( dett == 0 )
			return 0; // same
		else
			return (s-a).GetLen(); // distanced
	}
	t = dett / det;
	r = detr / det;
	float3 p1 = s + d*t;
	float3 q1 = a + b*r;
	return (p1-q1).GetLen();
}

float RaySegmentDistance(
		const float3 &s,const float3 &d,
		const float3 &a,const float3 &b,float& r)
{
	float t;
	float val = RayRayDistance(s,d,t,a,b,r);
	if( r>=0 && r<=1 ) return val;
	if( r<0 )
		return RayPointDistance(s,d,a);
	return RayPointDistance(s,d,a+b);
}

void CMouseMove::mouseMoveEvent( QMouseEvent * event )
{
	if( CSelection::Instance().size()==0 ) return;
	Matrix4f mWorld;
	CSelection::Instance().GetMatrix(mWorld);
	float s = GetPixelScale( float3(mWorld.GetPos()) );
	float3 p = float4(mWorld.GetPos()).xyz();
	float3 r = float4(mWorld.GetRg()).xyz()*s;
	float3 u = float4(mWorld.GetUp()).xyz()*s;
	float3 a = float4(mWorld.GetAt()).xyz()*s;

	float3 end = m_pCamera->Get3DPixel(float3(event->x(),event->y(),0.f));
	float3 start = m_pCamera->GetPosition();
	float3 dir = end-start;

	if( event->buttons() != Qt::NoButton )
	{
		float t,rr;
		float3 newP;
		switch(m_eState)
		{
		case ST_MARKX:
			RayRayDistance(start,dir,t,p,r,rr);
			newP = p+r*rr;
			break;
		case ST_MARKY:
			RayRayDistance(start,dir,t,p,u,rr);
			newP = p+u*rr;
			break;
		case ST_MARKZ:
			RayRayDistance(start,dir,t,p,a,rr);
			newP = p+a*rr;
			break;
		default:
			return;
		}
		mWorld = m_f16StartingMatrix;
		mWorld.Translate( newP - m_f3StartingPoint );
		CSelection::Instance().SetMatrix(mWorld);
	} else
	{
		float rx,dx = RaySegmentDistance(start,dir,p,r,rx);
		float ry,dy = RaySegmentDistance(start,dir,p,u,ry);
		float rz,dz = RaySegmentDistance(start,dir,p,a,rz);
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
	}
}
