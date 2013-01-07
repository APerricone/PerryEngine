#include "config.h"
#include "game.h"

#include "mymacros.h"

#define _USE_MATH_DEFINES
#include <math.h>

//#include <stdio.h>
#include <time.h>
//#include <stdlib.h>

#include "opengl.h"
#include <GL/glfw.h>

#include "font.h"
#include "scene.h"
#include "light.h"
//#include "fullscreenblit.h"
//#include "fxaa_interface.h"
//#include "camera.h"

#include "matrix.h"
#include "glperfcounter.h"
#include "rotatearoundcamera.h"
#include "window.h"

#include "image.h"
#include "pngloader.h"
#include "objloader.h"
#include "ssao.h"

#include "model.h"

void BuildFont();
void KillFont();

CGame::CGame()
{
	m_pScene = NULL;
	m_pCamera = NULL;
	m_bInit = false;
	m_bShowInfos = true;
}

void CGame::Deinit()
{
	glFinish();

	m_bInit = false;
	SAFE_DELETE(m_pScene);
	SAFE_DELETE(m_pCamera);

	CPngLoader::Unregister();
	CScene::DeinitStatic();

	KillFont();
	CGLPerfCounter::Deinit();
}

CGame::~CGame()
{
	Deinit();
}

CGame& CGame::GetInstance()
{
	static CGame instance;
	return instance;
}

void CGame::Resize( int width, int height )
{
	ILog::Message( "windows size %ix%i\n", width,height);
	glViewport(0,0,width,height);
	m_pScene->Resize(width,height);

	m_pCamera->m_fAspectRatio= (float)width/(float)height;

	CGLPerfCounter::SetupGraph(0,(float)height-15.f,(float)width,15.f,true);
	CGLPerfCounter::SetupLegend(20.f,(float)height-20.f,false,true);
}

void CGame::CreateFloor()
{
	CModel *pCube = CModel::CreateModel();
	unsigned int mainMesh = pCube->AddMesh();
	CDynamicMesh tmp;
	//tmp.AddBox( float3(-5.f,-2.f,-5.f),float3(5.f,-1.5f,5.f) );
	tmp.Normal( float3(0,1,0) );
	tmp.Texture( float2( 0, 0) ); tmp.Point( float3(-50.f,-1.5f,-50.f) );
	tmp.Texture( float2( 0,20) ); tmp.Point( float3(-50.f,-1.5f, 50.f) );
	tmp.Texture( float2(20, 0) ); tmp.Point( float3( 50.f,-1.5f,-50.f) );

	tmp.Texture( float2(20, 0) ); tmp.Point( float3( 50.f,-1.5f,-50.f) );
	tmp.Texture( float2( 0,20) ); tmp.Point( float3(-50.f,-1.5f, 50.f) );
	tmp.Texture( float2(20,20) ); tmp.Point( float3( 50.f,-1.5f, 50.f) );

	tmp.CreateMesh(*pCube->GetMesh(mainMesh),true,false,true);
	pCube->GetMesh(mainMesh)->RemoveCPUCopy();
	CMaterial::Data a;
	//a.f4Color.Set( 0.f,0.3f,0.8f,1.f);

	// grigia
	// 160,  16,  64
	// 128, 128, 160
	CImage cImage; 
	cImage.Create(2,2,3);
	unsigned char *pBits = cImage.GetBits();
	pBits[0]= pBits[11] = 160;
	pBits[1]= pBits[12] = 16;
	pBits[2]= pBits[13] = 64;
	pBits[3]= pBits[ 8] = 128;
	pBits[4]= pBits[ 9] = 128;
	pBits[5]= pBits[10] = 160;

	a.glDiffuseTex = cImage.GetTexture2D();

	glBindTexture(GL_TEXTURE_2D, a.glDiffuseTex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	pCube->SetMaterial(mainMesh,a);
}

void CGame::LoadDragon()
{
#define NAME "xyzrgb_dragon_15"
	FILE *f = fopen(NAME ".raw","rb");
	bool loaded = false;
	unsigned int mainMesh;
	CMaterial::Data a;
	if( f )
	{
		fclose(f);
		m_pSelection = CModel::CreateModel();
		m_pSelection->SetName(NAME);
		mainMesh = m_pSelection->AddMesh();
		if(m_pSelection->GetMesh(mainMesh)->LoadRaw(NAME".raw"))
		{
			loaded = true;
			m_pSelection->GetLastMesh()->RemoveCPUCopy();

			a.f4Color.Set( 0.7f,0.7f,0.7f,1.f);
			a.f4Specular.Set(1);
			m_pSelection->SetMaterial(mainMesh,a);
		}
	}
	if(!loaded)
	{
		CObjLoader obj;
		obj.LoadObj(NAME);
		m_pSelection = (CModel*)(CNode::GetByName(NAME));
		if( m_pSelection ) 
		{
			m_pSelection->GetLastMesh()->SaveRaw(NAME".raw");
			m_pSelection->GetLastMesh()->RemoveCPUCopy();
		}
	}/*/
	CModel* pSphere = CModel::CreateModel();
	mainMesh = pSphere->AddMesh();
	pSphere->GetMesh(mainMesh)->Begin();
	pSphere->GetMesh(mainMesh)->AddSphere(float3(),1.5f,32,64);
	//pSphere->GetMesh(mainMesh)->CalculateNormals();
	pSphere->GetMesh(mainMesh)->End(true);
	//pSphere->GetMesh(mainMesh)->SaveRaw("test.raw");
	//pSphere->GetMesh(mainMesh)->LoadRaw("test.raw");
	a.f4Color.Set( 0.8f,0.3f,0.3f,1.f);
	a.f4Specular.Set(1);
	pSphere->SetMaterial(mainMesh,a);
	//*/ 
	Matrix4f ll;
	ll.Identity();
	ll[0] = ll[5] = ll[10] = 0.05f;
	/*ll[4] = -0.05f;
	ll[2] = 0.05f; 
	ll[9] = -0.05f;
	/*ll[0] = 0.05f;
	ll[5] = 0.05f; 
	ll[10] = 0.05f;*/
	ll[13] = -1.6f; 
	if( m_pSelection )
		m_pSelection->SetLocal(ll);
}

void CGame::Init()
{
	time_t t;
	time(&t);
	srand((unsigned int)t);

	int2 size;
	glfwGetWindowSize(&size[0],&size[1]);

	ILog::Message( "windows size %ix%i\n", size[0],size[1]);
	
	BuildFont();

	//m_cKeyMan.Enable('W'); wire not uset more
	m_cKeyMan.Enable('1'); // show info
	//scene on/off
	m_cKeyMan.Enable('Z');
	m_cKeyMan.Enable('X');
	m_cKeyMan.Enable('C');
	m_cKeyMan.Enable('V');

	m_cKeyMan.Enable('E');
	m_cKeyMan.Enable('A');
	m_cKeyMan.Enable('S');

	m_cKeyMan.Enable(GLFW_KEY_F12);

	CPngLoader::Register();

	m_pScene = new CScene();
	m_pScene->Init();
	m_pScene->m_bUsePerCounter = true;
	// Create sample scene
	CreateFloor();
	LoadDragon();
	//*
	//m_pSelection = pSphere;
	m_pScene->SetCustomDrawStep(DrawSelection,this);
	m_pSelection = NULL;

	m_pCamera = new CRotateAroundCamera();
	m_pCamera->m_fAspectRatio= 1.f;
	m_pCamera->distance = 7.5f;
	m_pCamera->m_fNearPlane = 1;
	m_pCamera->m_fFarPlane = 100;
	m_pCamera->m_fFovY = 54;
	m_pCamera->alpha = 1.0f;
	m_pCamera->beta = 0.26f;
	m_pCamera->SetLookat(float3(0,0.5f,0.5f));

	m_pWindow = new CWindow(m_pScene->GetLight()->GetPosition());

	m_bInit = true;
	ILog::Message("\n");
}

struct RayHitTest
{
	RayHitTest(const float3& _s,const float3& _d) :
		m_pHit(NULL),
		start(_s),
		dir(_d),
		m_fDist(FLT_MAX)
		{ }

	CModel* m_pHit;
	float3 start,dir;
	float m_fDist;

	void operator()(CNode* p)
	{
		CModel *pModel = reinterpret_cast<CModel*>(p);
		if( !pModel->IsKindModel() ) return;
		float d = pModel->LineCollision(start,dir);
		if( d<m_fDist )
		{
			m_pHit = pModel;
			m_fDist = d;
		}
	}
};

void CGame::UpdateSelection()
{
	int2 mouse;
	int2 size;
	glfwGetWindowSize(&size[0],&size[1]);
	glfwGetMousePos(&mouse[0],&mouse[1]);
	float2 p;
	p[0] = (mouse[0] * 2.f / size[0]) - 1.f;
	p[1] = 1.f - (mouse[1] * 2.f / size[1]);

	float3 end = m_pCamera->Get3D(p);
	RayHitTest tmp(m_pCamera->GetPosition(), end - m_pCamera->GetPosition());
	CNode::ForAllNodeHierarchy(tmp);
	m_pSelection = tmp.m_pHit;
}

bool CGame::Update()
{
	if( lastTime!=0)
	{
		nTime++;
		tempTime+=lastTime;
		lastTime=0;
		if(nTime==60)
		{
			currentTime = tempTime/60.;
			nTime = 0;
			tempTime = 0;
		}
	}

	int3 mouse;
	bool mouseBtn= glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	glfwGetMousePos(&mouse[0],&mouse[1]);
	mouse[2] = glfwGetMouseWheel();
	m_pCamera->Update(mouse,mouseBtn);
	CScene::UpdateStatic();

	static bool mouseSel = false;
	mouseBtn= glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	if( !mouseSel && mouseBtn)
	{
		UpdateSelection();
	}



	m_cKeyMan.Update();
	if( m_cKeyMan.IsPressed('1') ) m_bShowInfos =!m_bShowInfos;
	if( m_cKeyMan.IsPressed('Z') ) m_pScene->m_bApplySsao = !m_pScene->m_bApplySsao;
	if( m_cKeyMan.IsPressed('X') ) m_pScene->m_bApplyColor = !m_pScene->m_bApplyColor;
	if( m_cKeyMan.IsPressed('C') ) m_pScene->m_bApplyLight = !m_pScene->m_bApplyLight;
	if( m_cKeyMan.IsPressed('V') ) m_pScene->m_bApplySpecular = !m_pScene->m_bApplySpecular;
	if( m_cKeyMan.IsPressed('E') ) m_pScene->ToggleFXAA();
	/*
	if( m_cKeyMan.IsPressed('A') ) m_pScene->GetSSAO()->radius += 0.001f;
	if( m_cKeyMan.IsPressed('S') ) m_pScene->GetSSAO()->radius -= 0.001f;
	/*/
	if( m_cKeyMan.IsDown('A') ) m_pScene->GetSSAO()->radius += 0.001f;
	if( m_cKeyMan.IsDown('S') ) m_pScene->GetSSAO()->radius -= 0.001f;
	//*/
	/*if( m_cKeyMan.IsPressed('W') )
	{
		static bool bWire = false;
		bWire = !bWire;
		glPolygonMode(GL_FRONT_AND_BACK, bWire? GL_LINE : GL_FILL);
	}*/

	if( m_cKeyMan.IsPressed(GLFW_KEY_F12) )
	{
		CMRT tmp;
		tmp.Init(4000,2250);
		tmp.AddRT();
		Resize(4000,2250);

		m_pScene->DrawRT(m_pCamera,&tmp);
		tmp.Reset();
		
		CImage *pScreen = CImage::CreateFromTexture2D( tmp.GetTexture(0) );
		//glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE, tex.GetBits() );
		//glBindTexture(GL_TEXTURE_2D,0);
		//glDisable(GL_TEXTURE_2D);
		pScreen->Save("test.png");
		delete pScreen;

		int width,height;
		glfwGetWindowSize(&width,&height);
		Resize(width,height);
		glDepthFunc(GL_LEQUAL);
	}

	return true;
}

void CGame::DrawSelection(void* _pThis)
{
	CGame* pThis = reinterpret_cast<CGame*>(_pThis);
	CModel *pSel = pThis->m_pSelection;
	if(pSel)
	{
		glBlendFunc(GL_ONE,GL_ONE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glColor4f(1,1,1,1);
		pSel->Draw(true,false);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}

	glBlendFunc(GL_ONE,GL_ONE);
	glColor4f(10,10,10,1);
	pThis->m_pWindow->Draw();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	float v = (float)(pow(0.5,2.2));
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

void CGame::Draw()
{
	if( !m_bInit ) return;

	CGLPerfCounter::Begin();
	//glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	int width,height;
	glfwGetWindowSize(&width,&height);
	m_pScene->DrawScreen(m_pCamera,width,height);
	
	//CGLPerfCounter::Mark("Scene");

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_pCamera->GetProj());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor4f(1,0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glColor4f(0,1,0,1);
	glVertex3f(0,0,0);
	glVertex3f(0,1,0);
	glColor4f(0,0,1,1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);
	glEnd();
	glEnable(GL_DEPTH_TEST);

	//write text
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1,1,0);
	glScalef(2.f/width,-2.f/height,1);
	glColor4f(1,1,1,1);

	glPrint(20,20,"fps %4.1f", 1.f/(float)(currentTime) );
	if( m_bShowInfos )
	{
		float y=20;
		glPrint(20,y+=20,"%s informations (1)", m_bShowInfos? "show" : "hide");
		glPrint(20,y+=20,"%s SSAO (Z)", m_pScene->m_bApplySsao? "show" : "hide");
		glPrint(20,y+=20,"%f ssao radius(A/A)", m_pScene->GetSSAO()->radius);
		glPrint(20,y+=20,"%s color (X)", m_pScene->m_bApplyColor? "show" : "hide");
		glPrint(20,y+=20,"%s light (C)", m_pScene->m_bApplyLight? "show" : "hide");
		glPrint(20,y+=20,"%s specular (V)", m_pScene->m_bApplySpecular? "show" : "hide");
		glPrint(20,y+=20,"%s FXAA (E)", m_pScene->GetFXAA()? "show" : "hide");
		glPrint(20,y+=20,"F12 Take screenshot");
	}	

	CGLPerfCounter::Mark("Print");

	CGLPerfCounter::PrintPreviousFrame(!m_bShowInfos);

	glfwSwapBuffers();
	CGLPerfCounter::Mark("Swap");
	CGLPerfCounter::End();

}
