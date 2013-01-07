#pragma once

#include "vector.h"
class CMesh;

// renderer of a fake window
class CWindow
{
public:
	CWindow(const float3& lDir);
	~CWindow();

	void Draw();

	void SetIntensity(float fInt) { m_fInt = fInt; }
	float GetIntensity() const { return m_fInt; }
private:
	CMesh* m_pMesh;

	float m_fInt;

	unsigned int m_glVertex;
	unsigned int m_glFragment;
	unsigned int m_glProgram;
};
