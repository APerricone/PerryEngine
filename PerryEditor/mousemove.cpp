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
	// 100 pixel len = 1 unit
	// 3 pixel width = 0.03 unit
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
	Draw(1.0f);
}

void CMouseMove::DrawOverScene()
{
	Draw(0.5f);
}

void CMouseMove::Draw(float alpha)
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
		glColor4f(1,0,0,alpha);
	else
		glColor4f(1,1,1,alpha);
	m_pArrow->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKY)
		glColor4f(0,1,0,alpha);
	else
		glColor4f(1,1,1,alpha);
	m_pArrow->ReDraw();
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	tmp.SetPos(p);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKZ)
		glColor4f(0,0,1,alpha);
	else
		glColor4f(1,1,1,alpha);
	m_pArrow->ReDraw();
	glPopMatrix();
	m_pArrow->EndDraw();

	m_pHalfCross->PreDraw();

	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
	glPushMatrix();
	if( m_eState != ST_MARKXY)
		glColor4f(1,0,0,alpha);
	else
		glColor4f(1,1,1,alpha);
	tmp.SetPos(p + (r+u) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();
	glPushMatrix();
	if( m_eState != ST_MARKXZ)
		glColor4f(1,0,0,alpha);
	else
		glColor4f(1,1,1,alpha);
	tmp.SetPos(p + (r+a) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();

	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glPushMatrix();
	if( m_eState != ST_MARKXY)
		glColor4f(0,1,0,alpha);
	else
		glColor4f(1,1,1,alpha);
	tmp.SetPos(p + (r+u) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();
	glPushMatrix();
	if( m_eState != ST_MARKYZ)
		glColor4f(0,1,0,alpha);
	else
		glColor4f(1,1,1,alpha);
	tmp.SetPos(p + (u+a) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();

	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	glPushMatrix();
	if( m_eState != ST_MARKXZ)
		glColor4f(0,0,1,alpha);
	else
		glColor4f(1,1,1,alpha);
	tmp.SetPos(p + (r+a) * 0.8f );
	glMultMatrixf(tmp);
	m_pHalfCross->ReDraw();
	glPopMatrix();
	glPushMatrix();
	if( m_eState != ST_MARKYZ)
		glColor4f(0,0,1,alpha);
	else
		glColor4f(1,1,1,alpha);
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

void CMouseMove::mouseMoveEvent( QMouseEvent * event )
{
	if( CSelection::Instance().size()==0 )
	{
		m_eState = ST_NONE;
		return;
	}
	Matrix4f mWorld;
	CSelection::Instance().GetMatrix(mWorld);
	float s = GetPixelScale( float3(mWorld.GetPos()) );
	float3 p = float3(mWorld.GetPos());
	float3 r = float3(mWorld.GetRg())*s;
	float3 u = float3(mWorld.GetUp())*s;
	float3 a = float3(mWorld.GetAt())*s;
	float3 pru = p+(r+u)*0.8f;
	float3 pra = p+(r+a)*0.8f;
	float3 pua = p+(u+a)*0.8f;


	Ray3f camRay(m_pCamera->Get3DRay(float3(event->x(),event->y(),0.f)));

	if( event->buttons() & Qt::LeftButton )
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
	}
}
