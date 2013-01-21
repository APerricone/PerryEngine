#include "scene.h"
#include "dynamicmesh.h"
#include "mymacros.h"
#include "material.h"
#include "light.h"
#include "opengl.h"
#include "mrt.h"
#include "fxaa_interface.h"
#include "gamma.h"
#include "fullscreenblit.h"
#include "camera.h"
#include "glperfcounter.h"
#include "font.h"
#include "ssao.h"

#include "node.h"
#include "model.h"

#include <stdio.h>

PFNGLPRINT glPrint;

CScene::CScene()
{
	m_pLight1 = 0;
	m_pGbuffers = 0;
	m_pOutput = 0;
	m_pFxaa = 0;
	m_pGamma = 0;
	m_pfnCustomDrawStep = 0;
	m_pSSAO = 0;

	m_bApplySsao = true;
	m_bApplyLight = true;
	m_bApplyColor = true;
	m_bApplySpecular = true;
	m_bUsePerCounter = false;
}

CScene::~CScene()
{
	SAFE_DELETE(m_pLight1);
	SAFE_DELETE(m_pGbuffers);
	SAFE_DELETE(m_pOutput);	
	SAFE_DELETE(m_pFxaa);
	SAFE_DELETE(m_pGamma);
	SAFE_DELETE(m_pSSAO);
}

void CScene::Resize(unsigned int i_iWidth,unsigned int i_iHeight)
{
	if(m_pGbuffers==0)
	{
		m_pGbuffers = new CMRT();
		m_pGbuffers->Init(i_iWidth,i_iHeight);
		m_pGbuffers->AddRT(); // normals
		m_pGbuffers->AddRT(); // albedo
		m_pGbuffers->AddRT(); // specular
		m_pGbuffers->AddDepth();
		m_pGbuffers->Check();
	} else
	{
		m_pGbuffers->Resize(i_iWidth,i_iHeight);
	}
	if(m_pOutput == 0)
	{
		m_pOutput = new CMRT();
		m_pOutput->Init(i_iWidth,i_iHeight);
		m_pOutput->AddRT(GL_RGB16F,GL_HALF_FLOAT,true); // output
		m_pOutput->AddDepthTex( m_pGbuffers->GetTextureDepth() );
		m_pOutput->Check();
	} else
	{
		m_pOutput->Resize(i_iWidth,i_iHeight);
	}
	m_pFxaa->Resize(i_iWidth,i_iHeight);
	//m_pGamma->Resize(i_iWidth,i_iHeight);
}

bool CScene::InitStatic()
{
	GLuint err = glewInit();
	if (GLEW_OK != err)
	{
		ILog::Error("glewInit failed: %s\n", glewGetErrorString(err));
		return false;
	}
	ILog::Message("initialized GLEW v.%i.%i.%i\n", GLEW_VERSION,GLEW_VERSION_MAJOR,GLEW_VERSION_MINOR);
	
	ILog::Message("OpenGL v.%s...", glGetString(GL_VERSION));
	// some check: OpenGL3
	if (GLEW_VERSION_3_3)
	{
		ILog::Message("OK! (OpenGL 3.3 needed)\n");
	}
	else
	{
		ILog::Error("FAIL! (OpenGL 3.3 needed)\n");
		return false;
	}
	// num render target
	GLint maxbuffers;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);
	if( maxbuffers >= 4 ) 
	{
		ILog::Message("support for %i RT... OK\n", maxbuffers);
	}
	else
	{
		ILog::Error("ERROR: insufficent number of RT supported\n");
		return false;
	}
	ILog::Message("\n");


	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glLightfv(GL_LIGHT0,GL_POSITION,&(float4(1,2.5f,2,0).GetNormalize()[0]));

	//BuildFont();

	bool bReturn(true);
	bReturn &= CFullScreenBlit::GetInstance()->Init();
	bReturn &= CLight::Compile();
	bReturn &= CMaterial::Compile();

	return bReturn;
}

void CScene::DeinitStatic()
{
	CLight::Deinit();
	CFullScreenBlit::GetInstance()->Deinit();
}

bool CScene::Init()
{
	//CGamma::CreateRefLutPng();
	m_pLight1 = new CLight(CLight::DIRECTIONAL);
	m_pLight1->CreateShadowTexture();
	m_pLight1->SetPosition(float3(-1.5f,-1.f,-2));
	m_pLight1->SetBox( float3(-5.f,-1.5f,-3.f),float3(5.f,3.5f,4.f) );
	m_pLight1->SetColor( float3(10,10,10) );

	m_pFxaa = new CFXAA();
	m_pFxaa->Init();
	
	m_pGamma = new CGamma();
	if(!m_pGamma->Init( false ))
	{
		ILog::Message("gamma Fail\n");
		return false;
	}
	//m_pGamma->LoadLut("****.png");
	//m_pGamma->CreateRefLutPng();

	ILog::Message("ssao..");
	m_pSSAO = new CSSAO();
	if(!m_pSSAO->Init())
	{
		ILog::Message("fail..");
		return false;
	}

	return true;
}

bool CScene::ReinitGamma(bool bNylon,bool bExposure,bool bFrame,bool bLut)
{
	if(m_pGamma==NULL) return false;
	return m_pGamma->Reinit(bNylon,bExposure,bFrame,bLut);
}

void CScene::UpdateStatic()
{
	CNode::GlobalUpdate();
}

void CScene::DrawScreen(Camera *i_pCamera,unsigned int i_iWidth,unsigned int i_iHeight)
{
	if( i_pCamera == NULL || i_iWidth == 0 || i_iHeight == 0) 
	{
		ILog::Error("Invalid DrawScreen call\n");
		return;
	}
	InternalDraw(i_pCamera,NULL,i_iWidth,i_iHeight);
}

void CScene::DrawRT(Camera *i_pCamera,CMRT* i_pRT)
{
	if( i_pCamera == NULL || i_pRT == NULL)
	{
		ILog::Error("Invalid DrawRT call\n");
		return;
	}	
	InternalDraw(i_pCamera,i_pRT,0,0);
}

void CScene::InternalDraw(Camera *i_pCamera,CMRT* i_pRT,unsigned int i_iWidth,unsigned int i_iHeight)
{
	//CModel::CheckVisibilities(i_pCamera->GetFrustum());
	m_pCurrentCamera = i_pCamera;
	m_pCurrentCamera->Setup();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	UpdateShadowMap();
	UpdateGBuffer();

	//m_pGamma->BeginDraw();
	m_pOutput->SetIt();
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);

	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1,1,0);
	glScalef(2.f,-2.f,1);
	glError();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	ShowSSAO();
	ShowLights1();
	ShowColors();
	ShowLights2();

	// make sure the depth buffer is not used like texture!
	//glFinish();
	glFlush(); 

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_pCurrentCamera->GetProj());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	if( m_pfnCustomDrawStep )
	{
		m_pfnCustomDrawStep(m_pCustomDrawStepParam);
		if( m_bUsePerCounter )
			CGLPerfCounter::Mark("Custom draw step");
	}
	
	m_pOutput->Reset();
	m_pOutput->UpdateMips();
	//m_pGamma->EndDraw();

	ShowGamma();
	
	if(i_pRT)
	{
		i_pRT->SetIt();
	} else
		glViewport(0,0,i_iWidth,i_iHeight);
	ShowFXAA();

	if(i_pRT)
		i_pRT->Reset();
}

void CScene::UpdateShadowMap()
{
	m_pLight1->SetRT(  );

	CNode::DrawAll(false,false);

	m_pLight1->resetRT();

	if( m_bUsePerCounter )
		CGLPerfCounter::Mark("Update shadow map");

}

void CScene::UpdateGBuffer()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_pCurrentCamera->GetProj());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CMaterial::SetIt();
	m_pGbuffers->SetIt();
	glDisable(GL_BLEND);
	
	CNode::DrawAll(false,true);

	CMaterial::Reset();
	m_pGbuffers->Reset();

	if( m_bUsePerCounter )
		CGLPerfCounter::Mark("Update GBuffer");
}

void CScene::ShowSSAO()
{
	if(m_bApplySsao)
		m_pSSAO->Draw(
			m_pCurrentCamera->m_fNearPlane,m_pCurrentCamera->m_fFarPlane,
			m_pCurrentCamera->GetProj(),m_pCurrentCamera->GetInvProj(),
			m_pGbuffers->GetTexture(0),m_pGbuffers->GetTextureDepth());

	if( m_bUsePerCounter )
		CGLPerfCounter::Mark("Show ssao");
}

void CScene::ShowLights1()
{
	CLight::BeginDiffuse(m_pGbuffers->GetTexture(0),m_pGbuffers->GetTextureDepth());
	if(m_bApplyLight)
		m_pLight1->DrawDiffuse( m_pCurrentCamera->GetInvProj() );
	CLight::End();

	if( m_bUsePerCounter )
		CGLPerfCounter::Mark("Show diffuse");
}

void CScene::ShowLights2()
{
	CLight::BeginSpecular(m_pGbuffers->GetTexture(0),m_pGbuffers->GetTexture(2),m_pGbuffers->GetTextureDepth());
	if(m_bApplySpecular)
		m_pLight1->DrawSpecular( m_pCurrentCamera->GetInvProj(), m_pCurrentCamera->GetPosition() );
	CLight::End();

	if( m_bUsePerCounter )
		CGLPerfCounter::Mark("Show specular");
}

void CScene::ShowColors()
{
	glBlendFunc(GL_ZERO,GL_SRC_COLOR);
	glEnable(GL_BLEND);
	m_pGbuffers->SetTexture(1);
	if(m_bApplyColor)
		CFullScreenBlit::GetInstance()->Draw();
	CMRT::ResetTextures();

	if( m_bUsePerCounter )
		CGLPerfCounter::Mark("Show Colors");
}

void CScene::ShowGamma()
{
	m_pFxaa->BeginDraw();
	m_pOutput->SetTexture(0);
	m_pGamma->Draw();

	if( m_bUsePerCounter )
		CGLPerfCounter::Mark("gamma");

	m_pFxaa->EndDraw();
}

void CScene::ShowFXAA()
{
	m_pFxaa->Draw();

	if( m_bUsePerCounter )
		CGLPerfCounter::Mark("fxaa");
}

void CScene::ToggleFXAA()
{
	m_pFxaa->m_bEnable =!m_pFxaa->m_bEnable;
}

bool CScene::GetFXAA()
{
	return m_pFxaa->m_bEnable;
}

void CScene::SetFXAA(bool b)
{
	m_pFxaa->m_bEnable = b;
}

CLight* CScene::GetLight()
{
	return m_pLight1; 
}
