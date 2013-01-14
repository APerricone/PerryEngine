#include "ssao.h"
#include "opengl.h"
#include "glsl.h"
#include "fullscreenblit.h"
#include "matrix.h"
#include <math.h>

CSSAO::CSSAO()
{
	m_glFragmentShader = 0;
	m_glProgram = 0;
	m_glIntenityUniform = 0;

	radius = 0.01f;
	SetIntensity(0.5f);
}

CSSAO::~CSSAO()
{
	SAFE_DELETE_GL_PROGRAM(m_glProgram);
	SAFE_DELETE_GL_SHADER(m_glFragmentShader);
}


bool CSSAO::Init()
{
	ILog::Message("\ncompiling ssao...\n");
	unsigned int commonVert = CFullScreenBlit::GetInstance()->GetVertexShader();
	m_glFragmentShader = CGLSL::CreateFragmentShaderFromFile("ssao.frag");
	m_glProgram = CGLSL::LinkProgram(commonVert,m_glFragmentShader);
	if(m_glFragmentShader==0 || m_glProgram==0) return false;

	glUseProgram( m_glProgram );
	glUniform1i(glGetUniformLocation(m_glProgram, "normalTexture"), 0);
	glUniform1i(glGetUniformLocation(m_glProgram, "depthTexture"), 1);
	m_glIntenityUniform = glGetUniformLocation(m_glProgram, "intensity");
	m_glZValues = glGetUniformLocation(m_glProgram, "zValues");
	m_glProjMatrixUniform = glGetUniformLocation(m_glProgram, "projMatrix");
	m_glInvProjMatrixUniform = glGetUniformLocation(m_glProgram, "invProjMatrix");

	glUseProgram( 0 );
	glError();
	return true;
}

float CSSAO::GetIntensity() const 
{
	return powf(m_fIntensity,1/2.2f); 
}

void CSSAO::SetIntensity(float f) 
{ 
	m_fIntensity = powf(f,2.2f); 
}

void CSSAO::Draw(float znear,float zfar,
	const Matrix4f& proj,const Matrix4f& invProj,
	unsigned int glNormal,unsigned int glDepth)
{
	glDisable(GL_BLEND);
	glUseProgram(m_glProgram);
	glUniform1f(m_glIntenityUniform, m_fIntensity);
	glUniform3f(m_glZValues,znear,zfar,radius);
	glUniformMatrix4fv(m_glProjMatrixUniform, 1,false, proj);
	glUniformMatrix4fv(m_glInvProjMatrixUniform, 1,false, invProj);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glNormal);
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glDepth);

	CFullScreenBlit::GetInstance()->Quad();
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
}
