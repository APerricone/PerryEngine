#include "rotatearoundcamera.h"

CRotateAroundCamera::CRotateAroundCamera() : Camera()
{
	distance = 10;
	alpha = 0;
	beta = 0;
	exMouse.Set(0);
}

void CRotateAroundCamera::Update(const int3& mouse,bool mouseBtn)
{
	if(mouseBtn)
	{
		alpha+=(exMouse.x()-mouse.x())/100.f;
		beta+=(mouse.y()-exMouse.y())/100.f;
		if(beta>1.56f) beta=1.56f;
		if(beta<-1.56f) beta=-1.56f;
	}
	if(exMouse.z()!=mouse.z())
	{
		distance += (exMouse.z()-mouse.z())*0.5f;
	}

	exMouse = mouse;
}

void CRotateAroundCamera::Setup()
{
	m_f3Up.x()=-sinf(beta)*sinf(alpha);
	m_f3Up.y()= cosf(beta);
	m_f3Up.z()=-sinf(beta)*cosf(alpha);
	m_f3Position.x()=m_f3LookAt.x()+distance*cosf(beta)*sinf(alpha);
	m_f3Position.y()=m_f3LookAt.y()+distance*sinf(beta);
	m_f3Position.z()=m_f3LookAt.z()+distance*cosf(beta)*cosf(alpha);
	Camera::Setup();
}
