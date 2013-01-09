#include "mouseselect.h"
#include "selection.h"
#include "node.h"
#include "model.h"
#include "dynamicmesh.h"
#include "opengl.h"
#include "editorcamera.h"

#include <QMultiMap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWidget>
#include <QApplication>
#include <QCursor>

float IMouseAction::GetPixelScale(const float3& pos)
{
	//float3 pS = m_pCamera->Get2DPixel( pos );
	//float3 ref = m_pCamera->Get2DPixel( (pos + m_pCamera->GetUp()) );
	//return 100.f / (pS.y() - ref.y());
	float test = m_pCamera->GetFactorPixel(pos);
	return 100.f / test; //(pS.y() - ref.y());
}

CMouseSelect::CMouseSelect(QObject* pParent) :
	IMouseAction(pParent)
{
	setObjectName(QString::fromUtf8("mouse_select"));
	setCheckable(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/edit/icons/select_icon.png"), QSize(), QIcon::Normal, QIcon::On);
	setIcon(icon);
	setText(QApplication::translate("MouseActions", "&Select", 0, QApplication::UnicodeUTF8));
	setShortcut(QApplication::translate("MouseActions", "W", 0, QApplication::UnicodeUTF8));

	CDynamicMesh oCreator;
	// 100 pixel len
	// 3 pixel width
	oCreator.AddCone(float3(0.f,0.f,0.f),0.015f,float3(0.f,0.f,1.f),0.015f,16,false,false);
	m_pAxe = new CMesh();
	oCreator.CreateMesh(*m_pAxe);

	m_qAddCursor = new QCursor(QPixmap(":/select/cursors/arrow_plus.png"), 2, 2);
	m_qRemCursor = new QCursor(QPixmap(":/select/cursors/arrow_minus.png"), 2, 2);
}

CMouseSelect::~CMouseSelect()
{
	delete m_pAxe;
	delete m_qAddCursor;
	delete m_qRemCursor;
}

void CMouseSelect::DrawInsideScene()
{
	Matrix4f mWorld;
	if( CSelection::Instance().size() )
		CSelection::Instance().GetMatrix(mWorld);
	else
		mWorld.Identity();
	float s = GetPixelScale( float3( mWorld.GetPos()) );
	m_f4CurrentPos = mWorld.GetPos(); m_f4CurrentPos[3] = 1.f;
	m_f4CurrentRight = mWorld.GetRg()*s;
	m_f4CurrentUp = mWorld.GetUp()*s;
	m_f4CurrentAt = mWorld.GetAt()*s;

	Draw(1.f);
}

void CMouseSelect::DrawOverScene()
{
	Draw(0.5f);
}

void CMouseSelect::Draw(float fAlpha)
{
	Matrix4f tmp;
	tmp.SetPos(m_f4CurrentPos);
	m_pAxe->PreDraw();
	float4& r = m_f4CurrentRight;
	float4& u = m_f4CurrentUp;
	float4& a = m_f4CurrentAt;

	glPushMatrix();
	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
	glMultMatrixf(tmp);
	glColor4f(1,0,0,fAlpha);
	m_pAxe->ReDraw();
	glPopMatrix();

	glPushMatrix();
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glMultMatrixf(tmp);
	glColor4f(0,1,0,fAlpha);
	m_pAxe->ReDraw();
	glPopMatrix();

	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	glMultMatrixf(tmp);
	glColor4f(0,0,1,fAlpha);
	m_pAxe->ReDraw();
	glPopMatrix();

	m_pAxe->EndDraw();
}

void CMouseSelect::Draw2D()
{

}

struct RayHitTest
{
	RayHitTest(const float3& _s,const float3& _d) :
		start(_s),
		dir(_d)
		{ }

	typedef QMultiMap<float,CNode*> Container;
	Container m_apHit;
	float3 start,dir;

	void operator()(CNode* p)
	{
		float d = p->LineCollision(start,dir);
		if( d!=FLT_MAX )
		{
			m_apHit.insert(d,p);
		}
	}
};

void CMouseSelect::keyPressEvent ( QKeyEvent * event )
{
	if( event->key() == Qt::Key_Shift ||
		event->key() == Qt::Key_Alt)
	{
		QWidget *pMain = QApplication::activeWindow();
		if( event->modifiers() & Qt::ShiftModifier )
			pMain->setCursor(*m_qAddCursor);
		else
		if( event->modifiers() & Qt::AltModifier )
			pMain->setCursor(*m_qRemCursor);
	}
}

void CMouseSelect::keyReleaseEvent( QKeyEvent * event )
{
	if( event->key() == Qt::Key_Shift ||
		event->key() == Qt::Key_Alt)
	{
		QWidget *pMain = QApplication::activeWindow();
		if( event->modifiers() & Qt::ShiftModifier )
			pMain->setCursor(*m_qAddCursor);
		else
		if( event->modifiers() & Qt::AltModifier )
			pMain->setCursor(*m_qRemCursor);
		else
			pMain->unsetCursor();
	}
}

void CMouseSelect::mousePressEvent( QMouseEvent * event )
{
	float3 end = m_pCamera->Get3DPixel(float3(event->x(),event->y(),0.f));
	RayHitTest tmp(m_pCamera->GetPosition(), end - m_pCamera->GetPosition());
	CNode::ForAllNodeHierarchy(tmp);
	RayHitTest::Container::Iterator i;
	CSelection& oSel = CSelection::Instance();
	CNode *pNewNode = NULL;
	bool next = false;
	for(i=tmp.m_apHit.begin();i!=tmp.m_apHit.end();++i)
	{
		if(next)
		{
			pNewNode = i.value();
			next = false;
		}
		if( oSel.contains(i.value()) )
			next = true;
	}
	if( next || pNewNode == NULL)
	{
		pNewNode = NULL;
		for(i=tmp.m_apHit.begin();i!=tmp.m_apHit.end();++i)
		{
			if( !oSel.contains(i.value()) )
			{
				pNewNode = i.value();
				break;
			}
		}
	}
	if( event->modifiers() & Qt::ShiftModifier )
	{
		if( pNewNode )
			oSel.append(pNewNode);
	} else
	if( event->modifiers() & Qt::AltModifier )
	{
		pNewNode = NULL;
		for(i=tmp.m_apHit.begin();i!=tmp.m_apHit.end();++i)
		{
			if( oSel.contains(i.value()) )
			{
				pNewNode = i.value();
				break;
			}
		}
		oSel.removeOne(pNewNode);
	} else
	{
		oSel.clear();
		if( pNewNode )
			oSel.append(pNewNode);
	}
}

void CMouseSelect::mouseReleaseEvent( QMouseEvent * event )
{

}

void CMouseSelect::mouseMoveEvent( QMouseEvent * event )
{

}
