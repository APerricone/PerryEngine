#pragma once

template<typename type>
class Matrix4;
typedef Matrix4<float> Matrix4f;

class CSSAO
{
public:
	CSSAO();
	~CSSAO();

	bool Init();
	void Draw(float znear,float zfar,
		const Matrix4f& proj,const Matrix4f& invProj,
		unsigned int glNormal,unsigned int glDepth);

	float GetIntensity() const;
	void SetIntensity(float f);

	float radius;
private:
	unsigned int m_glFragmentShader;
	unsigned int m_glProgram;
	unsigned int m_glIntenityUniform;
	unsigned int m_glZValues;
	unsigned int m_glProjMatrixUniform;
	unsigned int m_glInvProjMatrixUniform;

	float m_fIntensity;
};
