#pragma once

#include "camera.h"
#include "vector.h"
#include <QPoint>

#include "ray.h"


class QWheelEvent;
class QMouseEvent;

class CEditorCamera : public Camera
{
public:
    CEditorCamera();

	void resizeGL( int width, int height ) { m_iWidth = width; m_iHeight = height; m_fAspectRatio = (float)width / (float)height;}
	bool wheelEvent( QWheelEvent * event,const float3& center );
	bool mouseMoveEvent( QMouseEvent * event,const float3& center );

	void PrintInfo();

	float3 Get3DPixel(const float3& p2D) const;
	float3 Get2DPixel(const float3& p3D) const;
	float GetFactorPixel(const float3& p3D) const;

	Ray3f Get3DRay(const float3& p2D) const;

private:
	QPoint exPos;

	int m_iWidth;
	int m_iHeight;
	float m_fFovTextTime;
};
