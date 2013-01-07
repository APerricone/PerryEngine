#include "transformmanager.h"
#include "node.h"
#include "matrix.h"
#include "editorcamera.h"
#include "opengl.h"
#include "font.h"
#include "dynamicmesh.h"

CTransformManager::CTransformManager()
{
	m_pSelection = NULL;
	CDynamicMesh oCreator;
	// 100 pixel len
	// 3 pixel width
	// arrow 10 pixel 6 pixel width
	oCreator.AddCone(float3(0.f,0.f,0.f),0.015f,float3(0.f,0.f,0.9f),0.015f,16,false,false);
	oCreator.AddCone(float3(0.f,0.f,0.9f),0.03f,float3(0.f,0.f,1.f),0,16,false,false);
	m_pMoveArrow = new CMesh();
	oCreator.CreateMesh(*m_pMoveArrow);
	oCreator.Clear();

	// 40 pixel len
	// 3 pixel width
	// arrow 10 pixel 6 pixel width
	oCreator.AddCone(float3(0.f,0.f,-0.1f),0.015f,float3(0.f,0.f,0.1f),0.015f,16,false,false);
	oCreator.AddCone(float3(0.f,0.f,0.1f),0.03f,float3(0.f,0.f,0.2f),0,16,false,false);
	oCreator.AddCone(float3(0.f,0.f,-0.1f),0.03f,float3(0.f,0.f,-0.2f),0,16,false,false);
	m_pMoveHalfCross = new CMesh();
	oCreator.CreateMesh(*m_pMoveHalfCross);
	oCreator.Clear();
}

CTransformManager::~CTransformManager()
{
}

void CTransformManager::Draw()
{
	if( m_pSelection == NULL ) return;
	const Matrix4f& mWorld = m_pSelection->GetWorld();
	float3 pS = m_pCamera->Get2DPixel( float3(mWorld.GetPos()) );
	float3 ref = m_pCamera->Get2DPixel( float3(mWorld.GetPos()) + m_pCamera->GetUp() );
	float s = 100.f / (pS.y() - ref.y());
	float4 p = mWorld.GetPos(); p[3] = 1.f;
	float4 r = mWorld.GetRg()*s;
	float4 u = mWorld.GetUp()*s;
	float4 a = mWorld.GetAt()*s;
	m_af2TextPositions[0] = float2(pS);
	m_af2TextPositions[1] = float2(m_pCamera->Get2DPixel(float3(p+r*(1.07f))));
	m_af2TextPositions[2] = float2(m_pCamera->Get2DPixel(float3(p+u*(1.07f))));
	m_af2TextPositions[3] = float2(m_pCamera->Get2DPixel(float3(p+a*(1.07f))));
	m_pMoveArrow->PreDraw();
	Matrix4f tmp;
	tmp.SetPos(p);

	glPushMatrix();
	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
	glMultMatrixf(tmp);
	glColor4f(1,0,0,1.0f);
	m_pMoveArrow->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(1,0,0,0.5f);
	m_pMoveArrow->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	glMultMatrixf(tmp);
	glColor4f(0,1,0,1.0f);
	m_pMoveArrow->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(0,1,0,0.5f);
	m_pMoveArrow->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	tmp.SetPos(p);
	glMultMatrixf(tmp);
	glColor4f(0,0,1,1.0f);
	m_pMoveArrow->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(0,0,1,0.5f);
	m_pMoveArrow->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	m_pMoveArrow->EndDraw();

	m_pMoveHalfCross->PreDraw();
	glPushMatrix();
	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
	tmp.SetPos(p+(r+u)*0.8f);
	glMultMatrixf(tmp);
	glColor4f(1,0,0,1.0f);
	m_pMoveHalfCross->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(1,0,0,0.5f);
	m_pMoveHalfCross->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(u);
	tmp.SetUp(a);
	tmp.SetAt(r);
	tmp.SetPos(p+(r+a)*0.8f);
	glMultMatrixf(tmp);
	glColor4f(1,0,0,1.0f);
	m_pMoveHalfCross->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(1,0,0,0.5f);
	m_pMoveHalfCross->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	tmp.SetPos(p+(r+u)*0.8f);
	glMultMatrixf(tmp);
	glColor4f(0,1,0,1.0f);
	m_pMoveHalfCross->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(0,1,0,0.5f);
	m_pMoveHalfCross->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(a);
	tmp.SetUp(r);
	tmp.SetAt(u);
	tmp.SetPos(p+(u+a)*0.8f);
	glMultMatrixf(tmp);
	glColor4f(0,1,0,1.0f);
	m_pMoveHalfCross->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(0,1,0,0.5f);
	m_pMoveHalfCross->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	tmp.SetPos(p+(r+a)*0.8f);
	glMultMatrixf(tmp);
	glColor4f(0,0,1,1.0f);
	m_pMoveHalfCross->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(0,0,1,0.5f);
	m_pMoveHalfCross->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	tmp.SetRg(r);
	tmp.SetUp(u);
	tmp.SetAt(a);
	tmp.SetPos(p+(u+a)*0.8f);
	glMultMatrixf(tmp);
	glColor4f(0,0,1,1.0f);
	m_pMoveHalfCross->ReDraw();
	glDisable(GL_DEPTH_TEST);
	glColor4f(0,0,1,0.5f);
	m_pMoveHalfCross->ReDraw();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	m_pMoveHalfCross->EndDraw();

	glEnable(GL_DEPTH_TEST);
}

void CTransformManager::Draw2D()
{
	glColor3f(1,0,0);
	glPrint(m_af2TextPositions[1][0]-4,m_af2TextPositions[1][1]-7,"X");
	glColor3f(0,1,0);
	glPrint(m_af2TextPositions[2][0]-4,m_af2TextPositions[2][1]-7,"Y");
	glColor3f(0,0,1);
	glPrint(m_af2TextPositions[3][0]-4,m_af2TextPositions[3][1]-7,"Z");
}

void CTransformManager::mousePressEvent( QMouseEvent * event )
{

}

void CTransformManager::mouseReleaseEvent( QMouseEvent * event )
{

}

void CTransformManager::mouseMoveEvent( QMouseEvent * event )
{

}
