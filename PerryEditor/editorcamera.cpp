#include "editorcamera.h"
#include "font.h"

#include "vector.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QApplication>

CEditorCamera::CEditorCamera() : Camera()
{
	m_fFovTextTime = -1;
}

void CEditorCamera::PrintInfo()
{
#define TR(str) QApplication::translate("CEditorCamera", str, 0, QApplication::UnicodeUTF8).toUtf8().constData()
	static const float deltaTime = 0.033f;
	glColor4f(1,1,1,1);
	glPrint(0,0,TR("Perspective"));
	float y=20;
	if(m_fFovTextTime > 0)
	{
		if(m_fFovTextTime<1)
			glColor4f(1,1,1,m_fFovTextTime);
		else
			glColor4f(1,1,1,1);
		glPrint(5,y,TR("Current fov: %3.1f"), m_fFovY);
		y+=20;
		m_fFovTextTime -= deltaTime;
	}
}

bool CEditorCamera::wheelEvent( QWheelEvent * event,const float3& center)
{
	if(event->orientation() == Qt::Vertical)
	{
		if( (event->modifiers() & Qt::ControlModifier) != 0)
		{
			float oldFov = m_fFovY;
			float newFov = 	m_fFovY - event->delta() / 120.f;
			if( newFov > 0 && newFov<179)
				m_fFovY = newFov;

			float oldDist = dot((m_f3Position-center),GetAt());
			//	oldH = oldDist * tan(oldFov);
			//	newH = newDist * tan(newFov);
			//	oldDist * tan(oldFov) = newDist * tan(newFov)
			//	newDist = oldDist * tan(oldFov) / tan(newFov)
			float newDist = oldDist * tan(oldFov * M_PI / 360) / tan(newFov * M_PI / 360);
			float delta = newDist - oldDist;
			m_f3Position += GetAt() * delta;
			m_f3LookAt += GetAt() * delta;
			m_fFovTextTime = 2.f;
			return m_fFovY != oldFov;
		} else
		{
			m_f3Position += GetAt() * (event->delta() / 120.f);
			m_f3LookAt += GetAt() * (event->delta() / 120.f);
		}
		return (event->delta() != 0);
	}
	return false;
}

bool CEditorCamera::mouseMoveEvent( QMouseEvent * event,const float3& center )
{
	QPoint delta = exPos - event->pos();
	exPos = event->pos();
	if( (event->buttons() & Qt::MidButton) != 0)
	{
		if( (event->modifiers() & Qt::ControlModifier) != 0)
		{
			float alpha=(delta.x())/100.f;
			float beta=(delta.y())/100.f;
			float asinold = asinf(GetAt().y());
#define MAX_Y 0.99f
			if(asinold+beta>=asinf(MAX_Y))
			{
				beta = asinf(MAX_Y) - asinold;
			}
			if(asinold+beta<=asinf(-MAX_Y))
			{
				beta = asinf(-MAX_Y) - asinold;
			}
#undef MAX_Y
			Matrix4f rotY,rotX,totRot;
			rotY.RotateY(-alpha);
			rotX.RotateAxis(GetRg(),beta);
			totRot.Multiply(rotX,rotY);
			float3 translate = center - float3(totRot.Transform(float4(center,0)));
			totRot[12] = translate[0];
			totRot[13] = translate[1];
			totRot[14] = translate[2];

			m_f3LookAt = float3(totRot.Transform(float4(m_f3LookAt,1)));
			m_f3Position = float3(totRot.Transform(float4(m_f3Position,1)));
			m_f3Up.Set(0,1,0);
			Setup();
			return true;
		} else
		{
			float3 movement(0.f);
			movement += GetRg() * (delta.x() / 100.f);
			movement -= GetUp() * (delta.y() / 100.f);
			m_f3Position += movement;
			m_f3LookAt += movement;
			return dot(movement,movement) > 0;
		}
	}
	return false;
}

float3 CEditorCamera::Get3DPixel(const float3& p2D) const
{
	float3 p( p2D );
	p[0] = (p2D[0] * 2.f / m_iWidth) - 1.f;
	p[1] = 1.f - (p2D[1] * 2.f / m_iHeight);
	return Get3D(p);
}

float3 CEditorCamera::Get2DPixel(const float3& p3D) const
{
	float3 p( Get2D(p3D) );
	p[0] = (p[0]+1) * m_iWidth / 2.f;
	p[1] = (1-p[1]) * m_iHeight / 2.f;
	return p;
}

float CEditorCamera::GetFactorPixel(const float3& p3D) const
{
	return GetFactor(p3D) * m_iHeight;
}

Ray3f CEditorCamera::Get3DRay(const float3& p2D) const
{
	float3 end = Get3DPixel(p2D);
	float3 start = GetPosition();
	float3 direx = end-start;
	return Ray3f(start,direx);
}
