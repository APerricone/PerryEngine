#pragma once

#include "keymanager.h"

class CScene;
class CModel;
class CRotateAroundCamera;
class CWindow;

class CGame
{
	CGame();
	CGame(CGame&);
public:
	static CGame& GetInstance();
	virtual ~CGame();

	void Init();
	bool Update();
	void Draw();
	void Deinit();

	void Resize( int width, int height );

	void SetLastFrameTime( double t ) { lastTime = t; }
private:
	bool m_bInit;
	KeyManager m_cKeyMan;
	CScene* m_pScene;
	CRotateAroundCamera* m_pCamera;
	CModel *m_pSelection;
	CWindow* m_pWindow;
	static void DrawSelection(void*);

	double lastTime;
	double currentTime;
	double tempTime;
	int nTime;
	bool m_bShowInfos;

	void CreateFloor();
	void LoadDragon();

	void UpdateSelection();
};
