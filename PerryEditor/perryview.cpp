#include "perryview.h"
#include "mymacros.h"
#include "font.h"
#include "fullscreenblit.h"
#include "light.h"
#include "model.h"
#include "editorcamera.h"

#include "mouseactions.h"
#include "standardmouseactions.h"
#include "selection.h"

#include <QBasicTimer>
#include <QMouseEvent>
#include <float.h>

CPerryView::CPerryView(QWidget * parent) :
	QGLWidget(parent),
	m_qTimer(new QBasicTimer)
{
	m_pScene = NULL;
	m_pCamera = NULL;
	m_qRenderingOptions = NULL;
	setAutoBufferSwap(true);
	setMouseTracking(true);
	setFocusPolicy(Qt::WheelFocus);

	glPrint = StdGlPrint;
	m_bIsInitialized = false;
}

CPerryView::~CPerryView()
{
	SAFE_DELETE(m_qTimer);
	SAFE_DELETE(m_pScene);
	CScene::DeinitStatic();
}

void CPerryView::SampleScene()
{
	Matrix4f tmp;
	tmp.Identity();
	// Create sample scene
	CDynamicMesh creator;
	CModel *pCube = CModel::CreateModel();
	unsigned int mainMesh = pCube->AddMesh();
	creator.AddBox( float3(-5.f,-0.25f,-5.f),float3(5.f,0.25f,5.f) );
	creator.CreateMesh( *pCube->GetMesh(mainMesh), true);
	creator.Clear();
	pCube->GetMesh(mainMesh)->CalculateBoundings();
	tmp.SetPos(float4(0,-1.75f,0,1));
	pCube->SetLocal(tmp);
	CMaterial::Data a;
	a.f4Color.Set( 0.3f,0.3f,0.8f,1.f);
	pCube->SetMaterial(mainMesh,a);

	CModel* pSphere = CModel::CreateModel();
	mainMesh = pSphere->AddMesh();
	//creator.AddSphere(float3(0,0,0),1.5f,16,32);
	creator.AddTorus(float3(0,0,0),3,1.5f,16,32);
	creator.CreateMesh( *pSphere->GetMesh(mainMesh), true);
	creator.Clear();
	pSphere->GetMesh(mainMesh)->CalculateBoundings();
	tmp.SetPos(float4(5.f,0.f,0.f,1.f));
	pSphere->SetLocal(tmp);
	a.f4Color.Set( 0.8f,0.3f,0.3f,1.f);
	a.f4Specular.Set(1);
	pSphere->SetMaterial(mainMesh,a);
	// 30 fps
	m_qTimer->start(33,this);
}

void CPerryView::timerEvent(QTimerEvent *e)
{
	if( m_qRenderingOptions )
	{
		m_pScene->m_bApplySsao = m_qRenderingOptions->GetSSAOEnabled();
		m_pScene->m_bApplyLight = m_qRenderingOptions->GetDiffuseEnabled();
		m_pScene->m_bApplySpecular = m_qRenderingOptions->GetSpecularEnabled();
		m_pScene->m_bApplyColor = m_qRenderingOptions->GetColorEnabled();
		m_pScene->SetFXAA(m_qRenderingOptions->GetFXAAEnabled());

	}
	CScene::UpdateStatic();
	updateGL();
}

void CPerryView::initializeGL()
{
	QGLWidget::initializeGL();
	if(!CScene::InitStatic())
	{
		//return;
	}
	m_bIsInitialized = true;
	m_pScene = new CScene();
	m_pScene->Init();
	m_pScene->m_bUsePerCounter = false;
	SampleScene();
	m_pScene->SetCustomDrawStep(DrawEditor,this);

	m_pCamera = new CEditorCamera();
	m_pCamera->SetPosition(float3(10,5,-10));

	CMouseActions::SetCamera(m_pCamera);
	m_pStandardMouseActions = new CStandardMouseActions();
	m_pStandardMouseActions->InitActions();
}

void CPerryView::resizeGL( int width, int height )
{
	if(!m_bIsInitialized) return;
	QGLWidget::resizeGL(width,height);
	m_pScene->Resize(width,height);
	m_pCamera->resizeGL( width , height);
}

#pragma region CScene CALLBACK

void CPerryView::DrawEditor(void* pThis)
{
	CPerryView* pPerryView = reinterpret_cast<CPerryView*>(pThis);
	if(!pPerryView->IsInitialized()) return;

	bool bShowGrid(true);
	if( pPerryView->m_qRenderingOptions )
	{
		bShowGrid = pPerryView->m_qRenderingOptions->GetGridEnabled();
	}
	if( bShowGrid )
	{
		pPerryView->DrawGrid();
	}
	pPerryView->DrawBackground();

	CSelection::Instance().EvidenceSelection();
	CMouseActions::DrawInsideScene();
	glClear(GL_DEPTH_BUFFER_BIT);
	CMouseActions::DrawOverScene();

}

void CPerryView::DrawGrid()
{
	float v = (float)(pow(0.5,2.2));
	glColor4f(v,v,v,1);
	glBegin(GL_LINES);
	for(int i=-10;i<=10;i++)
	{
		if(i==0) glColor4f(0,0,v,1);
		glVertex3i(-10,0,i);
		glVertex3i( 10,0,i);
		if(i==0) glColor4f(v,0,0,1);
		glVertex3i(i,0,-10);
		glVertex3i(i,0, 10);
		if(i==0) glColor4f(v,v,v,1);
	}
	glEnd();
}

void CPerryView::DrawBackground()
{
	//glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	float v = (float)(pow(0.75,2.2));
	glColor4f(v,v,v,1);
	glVertex3f(-1,-1,1);
	glVertex3f( 1,-1,1);
	glVertex3f( 1, 1,1);
	glVertex3f(-1, 1,1);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

#pragma endregion

void CPerryView::paintGL()
{
	s_pCurrentView = this;

	QGLWidget::paintGL();
	if(!m_bIsInitialized) return;
	//pScene->Update();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	m_pScene->DrawScreen(m_pCamera, width(),height());

	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1,1,0);
	glScalef(2.f/width(),-2.f/height(),1);
	glDisable(GL_DEPTH_TEST);
	glColor4f(1,1,1,1);

	m_pCamera->PrintInfo();
	CMouseActions::Draw2D();

	glEnable(GL_DEPTH_TEST);
	s_pCurrentView = NULL;
}


#pragma region mouse

void CPerryView::wheelEvent( QWheelEvent * event )
{
	QGLWidget::wheelEvent(event);
	bool update;
	float3 c = CSelection::Instance().GetBoundingSphere().GetCenter();
	update = m_pCamera->wheelEvent(event,c);
	//if(update) updateGL();
}

void CPerryView::keyPressEvent ( QKeyEvent * event )
{
	QGLWidget::keyPressEvent(event);
	CMouseActions::keyPressEvent(event);
}

void CPerryView::keyReleaseEvent ( QKeyEvent * event )
{
	QGLWidget::keyReleaseEvent(event);
	CMouseActions::keyReleaseEvent(event);
}

void CPerryView::mouseMoveEvent( QMouseEvent * event )
{
	QGLWidget::mouseMoveEvent(event);
	CMouseActions::mouseMoveEvent(event);
	bool update;
	float3 c = CSelection::Instance().GetBoundingSphere().GetCenter();
	update = m_pCamera->mouseMoveEvent(event,c);
	//if(update) updateGL();
}

void CPerryView::mousePressEvent( QMouseEvent * event )
{
	if(event->button() == Qt::LeftButton)
	{
		CMouseActions::mousePressEvent(event);
	}
}


void CPerryView::mouseReleaseEvent( QMouseEvent * event )
{
	if(event->button() == Qt::LeftButton)
	{
		CMouseActions::mouseReleaseEvent(event);
	}
}

#pragma endregion
#pragma region glPrint

void CPerryView::StdGlPrint(float x,float y,const char *fmt, ...)
{
	if( s_pCurrentView == NULL ) return;
	QString strText;
	va_list	ap;
	va_start(ap, fmt);
	strText.vsprintf(fmt,ap);
	va_end(ap);
	s_pCurrentView->renderText(x,y+14,strText, QFont("Helvetica [Cronyx]", 14));
}

CPerryView* CPerryView::s_pCurrentView = NULL;

#pragma endregion
