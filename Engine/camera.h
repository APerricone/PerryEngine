#pragma once

#include "vector.h"
#include "matrix.h"

class Camera
{
public:
	float m_fNearPlane,m_fFarPlane;
	float m_fFovY;
	float m_fAspectRatio;
	bool m_bOrthogonal;

	Camera();

	virtual void Setup();

	float3 Get3D(const float3& p2D) const;
	float3 Get2D(const float3& p3D) const;
	// returns how much this point moves on the screen if it is added by m_f3Up
	float GetFactor(const float3& p3D) const;

	const float3& GetPosition() const { return m_f3Position; }
	const float3& GetRg() const { return m_f3Rg; }
	const float3& GetUp() const { return m_f3Up; }
	const float3& GetAt() const { return m_f3At; }

	const Matrix4f& GetProj() const { return m_f16ViewProjMat; }
	const Matrix4f& GetInvProj() const { return m_f16InvViewProjMat; }

	void SetPosition(const float3& pos)	{ m_f3Position = pos; }
	void SetLookAt(const float3& look)	{ m_f3LookAt = look; }
	void SetUp(const float3& i_up)		{ m_f3Up = i_up; }

protected:
	float3 m_f3Position;
	float3 m_f3LookAt;
	float3 m_f3Up;

private:
	float3 m_f3Rg,m_f3At;
	float m_fDistFactor;
	
	Matrix4f m_f16ViewMat;
	Matrix4f m_f16ViewProjMat;
	Matrix4f m_f16InvViewProjMat;
};
