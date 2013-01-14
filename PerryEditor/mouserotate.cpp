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
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKX)
		glColor4f(1,0,0,1.0f);
	else
		glColor4f(1,1,1,1.0f);
	m_pCircle->ReDraw();
	glPopMatrix();

	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	glMultMatrixf(tmp);
	if( m_eState != ST_MARKY)
		glColor4f(0,1,0,1.0f);
	else
		glColor4f(1,1,1,1.0f);
	m_pCircle->ReDraw();
	glPopMatrix();

	glPushMatrix();
	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
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
		m_f3StartingDir = (m_f3StartingPoint-float3(m_f16StartingMatrix.GetPos()));
		m_f3StartingDir.Normalize();
		float3 axe;
		switch(m_eState)
		{
		case ST_MARKX: axe = float3(m_f16StartingMatrix.GetRg()); break;
		case ST_MARKY: axe = float3(m_f16StartingMatrix.GetUp()); break;
		case ST_MARKZ: axe = float3(m_f16StartingMatrix.GetAt()); break;
		default:
			return;
		}
		m_f3StartingOrtho = cross( axe, m_f3StartingDir);
		m_f3StartingOrtho.Normalize();
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
	Ray3f camRay(m_pCamera->Get3DRay(float3(event->x(),event->y(),0.f)));

	if( event->buttons() & Qt::LeftButton )
	{
		float3 newP,axe;
		float t;
		switch(m_eState)
		{
		case ST_MARKX: axe=float3(m_f16StartingMatrix.GetRg()); break;
		case ST_MARKY: axe=float3(m_f16StartingMatrix.GetUp()); break;
		case ST_MARKZ: axe=float3(m_f16StartingMatrix.GetAt()); break;
		default:
			return;
		}
		if(!camRay.PlaneIntersection(float3(m_f16StartingMatrix.GetPos()),axe,t))
			return;
		axe.Normalize();
		newP = camRay.GetPoint(t) - float3(m_f16StartingMatrix.GetPos());
		newP.Normalize();
		float cosa(dot(m_f3StartingDir,newP));
		float sina(dot(m_f3StartingOrtho,newP));
		Matrix4f rot;
		rot.RotateAxis(axe,cosa,sina);
		float4 newPos(m_f16StartingMatrix.GetPos() -
					rot.Transform( m_f16StartingMatrix.GetPos()));
		newPos.w() = 1.f;
		rot.SetPos(newPos);
		Matrix4f final;
		final.Multiply(m_f16StartingMatrix,rot);
		CSelection::Instance().SetMatrix(final);
	} else
	{
		float s = GetPixelScale( float3(mWorld.GetPos()) );
		float3 p = float3(mWorld.GetPos());
		float3 r = float3(mWorld.GetRg());
		float3 u = float3(mWorld.GetUp());
		float3 a = float3(mWorld.GetAt());

		float t[3];
		bool b[3];
		b[0] = camRay.PlaneIntersection(p,r,t[0]);
		b[1] = camRay.PlaneIntersection(p,u,t[1]);
		b[2] = camRay.PlaneIntersection(p,a,t[2]);
		float dist = 1/s;
		m_eState = ST_NONE;
		for(int i=0;i<3;i++)
		{
			if( b[i] )
			{
				float3 c( camRay.GetPoint(t[i]) );
				float thisDist((c-p).GetLen());
				thisDist = fabsf(thisDist-s);
				if(thisDist<dist)
				{
					dist = thisDist;
					switch(i)
					{
					case 0: m_eState = ST_MARKX; break;
					case 1: m_eState = ST_MARKY; break;
					case 2: m_eState = ST_MARKZ; break;
					}
					m_f3StartingPoint = c;
				}
			}
		}
	}
}

