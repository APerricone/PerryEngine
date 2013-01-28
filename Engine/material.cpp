#include "material.h"
#include "opengl.h"
#include "mymacros.h"
#include "glsl.h"

#include "dynamicmesh.h"
#include "matrix.h"
#include "bestfitcube.h"
#include "image.h"

#include <stdio.h>

CMaterial CMaterial::instance;

CMaterial::CMaterial()
{
	m_glProgram = 0;
	m_glVertex = 0;
	m_glFragment = 0;
	m_pCubeTex = 0;
	m_glWhiteTex = 0;
}

CMaterial::~CMaterial()
{
	SAFE_DELETE( m_pCubeTex );
	SAFE_DELETE_GL_PROGRAM( m_glProgram );
	SAFE_DELETE_GL_SHADER( m_glVertex );
	SAFE_DELETE_GL_SHADER( m_glFragment );
	SAFE_DELETE_GL_TEXTURE( m_glWhiteTex );
}

bool CMaterial::CompileInternal()
{
	ILog::Message("\ncompiling material...\n");
	m_glVertex = CGLSL::CreateVertexShaderFromFile("material.vert");
	m_glFragment = CGLSL::CreateFragmentShaderFromFile("material.frag");
	m_glProgram = CGLSL::LinkProgram(m_glVertex,m_glFragment);
	glError();
	if( m_glVertex == 0 || m_glFragment == 0 || m_glProgram == 0)
	{
		return false;
	}
	
	m_pCubeTex = new CBestFitCube();
#ifndef _DEBUG
	m_pCubeTex->Init( 1024 );
#else
	m_pCubeTex->Init( 256 );
#endif
	glError();

	CImage pWhite;
	pWhite.Create(1,1,1);
	*(pWhite.GetBits()) = 255;
	m_glWhiteTex = pWhite.GetTexture2D();

	glUseProgram(m_glProgram);
	glUniform1i(glGetUniformLocation(m_glProgram, "diffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(m_glProgram, "bestFitTexture"), 1);
	glUseProgram( 0 );
	glError();
	return true;
}

void CMaterial::SetItInternal()
{
	glUseProgram( m_glProgram );
	glError();

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_pCubeTex->m_glTexture);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
}//*/

void CMaterial::UpdateInternal()
{
	return; 
	//opengl driver automatic set this uniforms
	Matrix4f projMat;
	glGetFloatv(GL_PROJECTION_MATRIX, projMat);
	Matrix4f modelViewMat;
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMat);

	glUseProgram( m_glProgram );
	glError();

	Matrix4f projmodelViewMat;
	projmodelViewMat.Multiply(projMat, modelViewMat);
	glUniformMatrix4fv(glGetUniformLocation(m_glProgram, "gl_ModelViewProjectionMatrix"), 
		1,false, projmodelViewMat);
	glError();

	Matrix3f normalMatrix,temp;
	modelViewMat.GetRotatePart(normalMatrix);
	normalMatrix.GetInvert(temp);
	temp.GetTranspose(normalMatrix);
	glUniformMatrix3fv(glGetUniformLocation(m_glProgram, "gl_NormalMatrix"), 
		1,false, normalMatrix);
	glError();
}

void CMaterial::ResetInternal()
{
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE0);

	glUseProgram( 0 );
	glError();
}

void CMaterial::SetDataInternal(const CMaterial::Data& i_data)
{
	// TODO: move it inside CMaterial::Data initialization
	float4 newSpec;
	newSpec.x() = powf(i_data.f4Specular.x(),2.2f);
	newSpec.y() = powf(i_data.f4Specular.y(),2.2f);
	newSpec.z() = powf(i_data.f4Specular.z(),2.2f);
	newSpec.w() = i_data.f4Specular.w();
	float4 newDiff;
	newDiff.x() = powf(i_data.f4Color.x(),2.2f);
	newDiff.y() = powf(i_data.f4Color.y(),2.2f);
	newDiff.z() = powf(i_data.f4Color.z(),2.2f);
	newDiff.w() = i_data.f4Color.w();
	float specI = dot(newSpec,float4(0.299f,0.587f,0.114f,0.f));
	float diffI = dot(newSpec,float4(0.299f,0.587f,0.114f,0.f));
	float f =(specI+diffI);
	if( f > 1.0 )
	{
		newDiff *= (1-specI)/diffI;
		newDiff.w() = i_data.f4Color.w();
	}
	glColor4fv( newDiff.operator const float *() );
	glMultiTexCoord4fv( GL_TEXTURE1, newSpec.operator const float *() );
	if( i_data.glDiffuseTex )
	{
		glBindTexture(GL_TEXTURE_2D, i_data.glDiffuseTex);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_glWhiteTex);
	}
}

void CMaterial::Data::Destroy()
{
	SAFE_DELETE_GL_TEXTURE(glDiffuseTex);
}
