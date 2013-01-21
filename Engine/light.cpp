#include "light.h"
#include "opengl.h"
#include "glsl.h"
#include "fullscreenblit.h"
#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>

unsigned int CLight::m_glFragmentDiffuse;
unsigned int CLight::m_glProgramDiffuse;
unsigned int CLight::m_glPosDirDiffuse;
unsigned int CLight::m_glColorDiffuse;

unsigned int CLight::m_glFragmentSpecular;
unsigned int CLight::m_glProgramSpecular;
unsigned int CLight::m_glPosDirSpecular;
unsigned int CLight::m_glColorSpecular;

CLight::CLight(eLightType m_eType) : m_eType(m_eType)
{
}

CLight::~CLight()
{
}

void CLight::Deinit()
{
	SAFE_DELETE_GL_PROGRAM(m_glProgramDiffuse);
	SAFE_DELETE_GL_PROGRAM(m_glProgramSpecular);
	SAFE_DELETE_GL_SHADER(m_glFragmentDiffuse);
	SAFE_DELETE_GL_SHADER(m_glFragmentSpecular);
}

bool CLight::Compile()
{
	ILog::Message("\ncompiling light...\n");
	unsigned int uiVertex = CFullScreenBlit::GetInstance()->GetVertexShader();
	m_glFragmentDiffuse = CGLSL::CreateFragmentShaderFromFiles(3,
		"light.start.frag",
		//"light.hard.frag",
		//"light.pcf.frag",
		"light.pcss.frag",
		"light.final.frag");
	m_glProgramDiffuse = CGLSL::LinkProgram(uiVertex,m_glFragmentDiffuse);
	m_glPosDirDiffuse = glGetUniformLocation(m_glProgramDiffuse, "lightDirPosType");
	m_glColorDiffuse = glGetUniformLocation(m_glProgramDiffuse, "lightCol");


	m_glFragmentSpecular = CGLSL::CreateFragmentShaderFromFiles(3,
		"light.start.frag",
		"light.hard.frag",
		"light.specular.frag");
	m_glProgramSpecular = CGLSL::LinkProgram(uiVertex,m_glFragmentSpecular);
	m_glPosDirSpecular = glGetUniformLocation(m_glProgramSpecular, "lightDirPosType");
	m_glColorSpecular = glGetUniformLocation(m_glProgramSpecular, "lightCol");

	if( m_glProgramDiffuse == 0 ||
		m_glProgramSpecular == 0 ||
		m_glFragmentDiffuse == 0 ||
		m_glFragmentSpecular == 0 )
	{
		return false;
	}

	glUseProgram(m_glProgramDiffuse);
	glUniform1i(glGetUniformLocation(m_glProgramDiffuse, "normalTexture"), 0);
	glUniform1i(glGetUniformLocation(m_glProgramDiffuse, "depthTexture"), 1);
	glUniform1i(glGetUniformLocation(m_glProgramDiffuse, "shadowTexture"), 2);

	glUseProgram(m_glProgramSpecular);
	glUniform1i(glGetUniformLocation(m_glProgramSpecular, "normalTexture"), 0);
	glUniform1i(glGetUniformLocation(m_glProgramSpecular, "depthTexture"), 1);
	glUniform1i(glGetUniformLocation(m_glProgramSpecular, "specularTexture"), 2);
	glUniform1i(glGetUniformLocation(m_glProgramSpecular, "shadowTexture"), 3);

	glUseProgram( 0 );
	glError();
	return true;
}

void CLight::BeginDiffuse(unsigned int glNormal,unsigned int glDepth)
{
	glBlendFunc(GL_ONE,GL_ONE);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glNormal);
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glDepth);
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);
	glError();
}

void CLight::BeginSpecular(unsigned int glNormal,unsigned int glSpecular,unsigned int glDepth)
{
	glColor4f(1,1,1,1);
	glBlendFunc(GL_ONE,GL_ONE);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glNormal);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glDepth);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glSpecular);

	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	
	glError();
}

void CLight::End()
{
	glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0);
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);

	glError();
}

void CLight::SetPosition(const float3& pos)
{
	if(m_eType == DIRECTIONAL)
	{
		m_f3Position = -pos.GetNormalize(); 
	} else
		m_f3Position = pos;
}

void CLight::DrawDiffuse(const float* invProj)
{
	glColor4f(1,1,1,1);
	glUseProgram(m_glProgramDiffuse);
	float4 v(m_f3Position, m_eType == POINT? 0.f:1.f);
	glUniform4fv(m_glPosDirDiffuse, 1, (float*)(v));
	glUniform3fv(m_glColorDiffuse, 1, (float*)(m_f3Color));
	glActiveTexture(GL_TEXTURE2);
	m_cShadow.SetTextureDepth();
	glUniform2f(glGetUniformLocation(m_glProgramDiffuse, "zValues"), 
		m_fZNear,m_fZFar);
	glUniformMatrix4fv(glGetUniformLocation(m_glProgramDiffuse, "lightMatrix"), 
		1,false, m_f16ProjectMatrix);
	glUniformMatrix4fv(glGetUniformLocation(m_glProgramDiffuse, "invProjMatrix"), 
		1,false, invProj);
	glError();

	CFullScreenBlit::GetInstance()->Quad();

	glUseProgram( 0 );
	glError();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CLight::DrawSpecular(const float* invProj,const float3& camPos)
{
	glColor4f(1,1,1,1);
	glUseProgram(m_glProgramSpecular);
	glError();

	float4 v(m_f3Position, m_eType == POINT? 0.f:1.f);
	glUniform4fv(m_glPosDirSpecular, 1, (float*)(v));
	glUniform3fv(m_glColorSpecular, 1, (float*)(m_f3Color));
	glUniform3fv(glGetUniformLocation(m_glProgramSpecular, "camPos"), 1, camPos.operator const float *());
	glError();

	glActiveTexture(GL_TEXTURE3);
	m_cShadow.SetTextureDepth();
	glUniformMatrix4fv(glGetUniformLocation(m_glProgramSpecular, "lightMatrix"), 
		1,false, m_f16ProjectMatrix);
	glUniformMatrix4fv(glGetUniformLocation(m_glProgramSpecular, "invProjMatrix"), 
		1,false, invProj);
	glError();

	CFullScreenBlit::GetInstance()->Quad();

	glUseProgram( 0 );
	glError();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CLight::CreateShadowTexture()
{
	m_cShadow.Init(256,256);
	//m_cShadow.AddRT();
	//m_cShadow.AddRT();
	m_cShadow.AddDepth();
}

void CLight::SetBox(const float3& _min,const float3& _max)
{
	m_f3Min = _min;
	m_f3Max = _max;
}

void CLight::UpdateProjMatrix()
{
	if( m_eType == POINT )
	{
		ILog::Error("CLight::UpdateProjMatrix() point light not implemented!\n");
	}
	float3 rg = float3(0,1,0) ^ m_f3Position;
	rg.Normalize();
	float3 up = m_f3Position ^ rg;
	up.Normalize();
	float3 dim = (m_f3Max-m_f3Min)*0.5f;
	float maxRight,maxUp;
	float distance,maxAt=-1e8;
	maxRight=maxUp=-1e8;
	for(int i=0;i<8;i++)
	{
		float3 d;
		d.x() = i&1? dim.x() : -dim.x();
		d.y() = i&2? dim.y() : -dim.y();
		d.z() = i&4? dim.z() : -dim.z();

		distance=dot(d,rg);
		if(distance>maxRight) maxRight=distance;

		distance=dot(d,up);
		if(distance>maxUp) maxUp=distance;

		distance=dot(d,m_f3Position);
		if(distance>maxAt) maxAt=distance;
	}
	maxRight *= 1.02f;
	maxUp *= 1.02f;
	maxAt *= 2;
	glLoadIdentity();
	m_fZNear = maxAt;
	m_fZFar = maxAt*3;
	glOrtho(
		-maxRight,maxRight,
		-maxUp,maxUp,
		m_fZNear,m_fZFar);
	float3 center = (m_f3Min+m_f3Max)*0.5f;
	float3 position = center + m_f3Position * maxAt*2.f;
	gluLookAt(position.x(),position.y(),position.z(),center.x(),center.y(),center.z(),up.x(),up.y(),up.z());

	glGetFloatv(GL_PROJECTION_MATRIX, m_f16ProjectMatrix);
	m_f16ProjectMatrix.GetInvert(m_f16InvProjectMatrix);
}

void CLight::SetRT()
{
	m_cShadow.SetIt();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	UpdateProjMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(4.,4.);
}

void CLight::resetRT()
{
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0,0.0);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glError();
	
	m_cShadow.Reset();
}

void CLight::DrawDebug()
{
	float4 p[12];
	
	p[0] = m_f16InvProjectMatrix.Transform( float4(-1,-1,-1,1) );
	p[1] = m_f16InvProjectMatrix.Transform( float4( 1,-1,-1,1) );
	p[2] = m_f16InvProjectMatrix.Transform( float4(-1, 1,-1,1) );
	p[3] = m_f16InvProjectMatrix.Transform( float4( 1, 1,-1,1) );
	p[4] = m_f16InvProjectMatrix.Transform( float4(-1,-1,1,1) );
	p[5] = m_f16InvProjectMatrix.Transform( float4( 1,-1,1,1) );
	p[6] = m_f16InvProjectMatrix.Transform( float4(-1, 1,1,1) );
	p[7] = m_f16InvProjectMatrix.Transform( float4( 1, 1,1,1) );

	p[8] = m_f16InvProjectMatrix.Transform( float4( 0,0,0,1) );
	p[9] = m_f16InvProjectMatrix.Transform( float4( 1,0,0,1) );
	p[10] = m_f16InvProjectMatrix.Transform( float4( 0,1,0,1) );
	p[11] = m_f16InvProjectMatrix.Transform( float4( 0,0,1,1) );

	for(int i=0;i<12;i++) p[i] /= p[i].w();
	glBegin(GL_LINES);
	glColor4f(1,1,1,1);
#define LINE(a,b) glVertex4fv(p[a]); glVertex4fv(p[b]); 

	LINE(0,1);
	LINE(2,3);
	LINE(4,5);
	LINE(6,7);
	LINE(0,2);
	LINE(1,3);
	LINE(4,6);
	LINE(5,7);
	LINE(0,4);
	LINE(1,5);
	LINE(2,6);
	LINE(3,7);

	LINE(8,9)
	LINE(8,10)
	LINE(8,11)

	glEnd();
}
