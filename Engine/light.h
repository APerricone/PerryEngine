#pragma once

#include "vector.h"
#include "matrix.h"
#include "mrt.h"

class CLight
{
public:
	enum eLightType
	{
		POINT,
		DIRECTIONAL,
		NRTYPE = 2
	};
	CLight(eLightType m_eType);
	virtual ~CLight();
	void SetPosition(const float3& pos);
	void SetColor(const float3& c) { m_f3Color = c; }

	static bool Compile();
	static void Deinit();

	static void BeginDiffuse(unsigned int glNormal,unsigned int glDepth);
	static void BeginSpecular(unsigned int glNormal,unsigned int glSpecular,unsigned int glDepth);
	static void End();

	virtual void CreateShadowTexture();
	void SetRT();
	void resetRT();

	void DrawDiffuse(const float* invProj);
	void DrawSpecular(const float* invProj,const float3& camPos);

	const eLightType GetType() const { return m_eType; }
	const float3& GetPosition() const { return m_f3Position; }
	const float3& GetColor() const { return m_f3Color; }

	void SetBox(const float3& _min,const float3& _max);

	void DrawDebug();

protected:
	eLightType m_eType;

	float3 m_f3Min,m_f3Max;
	float3 m_f3Position;
	float3 m_f3Color;
	float m_fZNear,m_fZFar;
	Matrix4f m_f16ProjectMatrix;
	Matrix4f m_f16InvProjectMatrix;
	virtual void UpdateProjMatrix();

	// openGL stuff
	static unsigned int m_glFragmentDiffuse;
	static unsigned int m_glProgramDiffuse;
	static unsigned int m_glFragmentSpecular;
	static unsigned int m_glProgramSpecular;
	static unsigned int m_glPosDirDiffuse;
	static unsigned int m_glPosDirSpecular;

private:
	CMRT m_cShadow;
};
