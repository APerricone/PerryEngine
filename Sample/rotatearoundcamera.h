#pragma once

#include "camera.h"

class CRotateAroundCamera : public Camera
{
public:
	float distance;
	float alpha,beta;

	CRotateAroundCamera();
	void Setup();

	void Update(const int3& mouse,bool mouseBtn);
	void SetLookat(const float3& i_f3Look) { m_f3LookAt = i_f3Look; }
private:
	int3 exMouse;
};
