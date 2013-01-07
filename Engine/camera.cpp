#include "camera.h"

#include "opengl.h"
#include <memory.h>
#include "matrix.h"

#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera()
{
	m_fNearPlane = 1;
	m_fFarPlane = 100;
	m_fFovY = 54;
	m_fAspectRatio = 1;
	m_bOrthogonal = false;
	m_f3Position.Set(5);
	m_f3LookAt.Set(0);
	m_f3Up.Set(0,1,0);
}

void Camera::Setup()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(m_bOrthogonal)
	{
		m_fDistFactor = 1.f;
		glOrtho(
			-m_fFovY*m_fAspectRatio*0.5f,m_fFovY*m_fAspectRatio*0.5f,
			-m_fFovY*0.5f,m_fFovY*0.5f,m_fNearPlane,m_fFarPlane);
	}
	else
	{
		m_fDistFactor = 1.f/tanf(m_fFovY*(float)(M_PI)/360.f);
		gluPerspective(m_fFovY,m_fAspectRatio,m_fNearPlane,m_fFarPlane);
	}
	glGetFloatv(GL_PROJECTION_MATRIX, m_f16ViewMat);
	gluLookAt(m_f3Position.x(),m_f3Position.y(),m_f3Position.z(),m_f3LookAt.x(),m_f3LookAt.y(),m_f3LookAt.z(),m_f3Up.x(),m_f3Up.y(),m_f3Up.z());

	//cotfov.y() = 1.f/tanf(m_fFovY*M_PI/360.f);
	//cotfov.x() = cotfov.y()/aspectRatio;
	m_f3At=m_f3LookAt-m_f3Position;	m_f3At.Normalize();
	m_f3Rg=m_f3At^m_f3Up;			m_f3Rg.Normalize();
	m_f3Up=m_f3Rg^m_f3At;			m_f3Up.Normalize();

	glGetFloatv(GL_PROJECTION_MATRIX, m_f16ViewProjMat);
	m_f16ViewProjMat.GetInvert(m_f16InvViewProjMat);
}

float3 Camera::Get2D(const float3& p3D) const
{
	float4 test1 = m_f16ViewProjMat.Transform(float4(p3D,1.0f));
	test1 *= 1/test1.w();
	return test1.xyz();
}

float3 Camera::Get3D(const float3& p2D) const
{	
	float4 test1 = m_f16InvViewProjMat.Transform(float4(p2D,1.0f));
	test1 *= 1/test1.w();
	return test1.xyz();
}

float Camera::GetFactor(const float3& p3D) const
{
	float4 test1 = m_f16ViewProjMat.Transform(float4(p3D,1.0f));
	return m_fDistFactor/(2*test1.w());
}
