#pragma once

#include "mrt.h"

class CFXAA
{
public:
	CFXAA();
	~CFXAA();

	void Init();
	void Deinit();
	void Resize(unsigned int i_width,unsigned int i_height);
	void Draw();

	void BeginDraw();
	void EndDraw();

	bool m_bEnable;
private:
	unsigned int m_glFragmentShader;
	unsigned int m_glProgram;

	CMRT m_oSrc;
};
