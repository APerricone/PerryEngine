#pragma once

#include <vector>
#include "dynamicmesh.h"
#include "material.h"

class CLight;
class CMRT;
class Camera;
class CGamma;
class CBallGrid;
class CFXAA;
class CSSAO;

typedef void CustomDrawStep(void*);

class CScene
{
public:
	CScene();
	~CScene();

	void Resize(unsigned int i_iWidth,unsigned int i_iHeight);

	static bool InitStatic();
	static void UpdateStatic();
	static void DeinitStatic();
	bool Init();
	bool ReinitGamma(bool bNylon,bool bExposure,bool bFrame,bool bLut);
	void DrawScreen(Camera *i_pCamera,unsigned int i_iWidth,unsigned int i_iHeight);
	void DrawRT(Camera *i_pCamera,CMRT* i_pRT);

	CustomDrawStep* GetCustomDrawStep() { return m_pfnCustomDrawStep; }
	void* GetCustomDrawStepParam() { return m_pCustomDrawStepParam; }
	void SetCustomDrawStep(CustomDrawStep* i_pFn,void* i_pParam) { m_pfnCustomDrawStep = i_pFn; m_pCustomDrawStepParam=i_pParam; }
	
	CLight* GetLight();
	CSSAO* GetSSAO() { return m_pSSAO; }
	void ToggleFXAA();
	bool GetFXAA();
	void SetFXAA(bool b);
	
	bool m_bApplySsao;
	bool m_bApplySpecular;
	bool m_bApplyColor;
	bool m_bApplyLight;

	bool m_bUsePerCounter;
private:
	// gamma applier
	CSSAO *m_pSSAO;
	// the light
	// TODO: use a list
	CLight *m_pLight1;
	// the g-buffers
	CMRT *m_pGbuffers;
	// the HDR output
	CMRT *m_pOutput;
	// gamma applier
	CGamma *m_pGamma;
	// fxaa applier
	CFXAA *m_pFxaa;
	// 
	Camera *m_pCurrentCamera;

	void InternalDraw(Camera *i_pCamera,CMRT* i_pRT,unsigned int i_iWidth,unsigned int i_iHeight);

	CustomDrawStep* m_pfnCustomDrawStep;
	void* m_pCustomDrawStepParam;

	void UpdateShadowMap();
	void UpdateGBuffer();

	void ShowSSAO();
	void ShowLights1();
	void ShowColors();
	void ShowLights2();
	
	void ShowGamma();
	void ShowFXAA();
};

