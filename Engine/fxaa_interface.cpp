#include "fxaa_interface.h"
#include "opengl.h"
#include "glsl.h"
#include "fullscreenblit.h"

#include <stdio.h>

CFXAA ::CFXAA ()
{
	m_glFragmentShader	= 0; 
	m_glProgram			= 0; 
	m_bEnable			= true;
}

CFXAA ::~CFXAA ()
{
	Deinit();
}


void CFXAA::Deinit()
{
	SAFE_DELETE_GL_PROGRAM(m_glProgram);
	SAFE_DELETE_GL_SHADER(m_glFragmentShader);
}

void CFXAA::BeginDraw()
{
	m_oSrc.SetIt();
}

void CFXAA::EndDraw()
{
	m_oSrc.Reset();
}

void CFXAA::Resize(unsigned int i_width,unsigned int i_height)
{
	m_oSrc.Resize(i_width,i_height);
	glUseProgram( m_glProgram );
	glUniform2f(glGetUniformLocation(m_glProgram, "rcpFrame"), 
			1.f/i_width,1.f/i_height);
	glUseProgram( 0 );
	glError();
}

void CFXAA::Init()
{
	ILog::Message("\ncompiling fxaa...\n");
	unsigned int commonVert = CFullScreenBlit::GetInstance()->GetVertexShader();
	char* fxaa_frag = CGLSL::LoadShader("fxaa.frag");
	const char *other = "#version 150\n";
	const char *sources[] = { other, fxaa_frag};
	m_glFragmentShader = CGLSL::CreateFragmentShader(2,sources);
	delete [] fxaa_frag;
	m_glProgram = CGLSL::LinkProgram(commonVert,m_glFragmentShader);
	glUseProgram( m_glProgram );
	glUniform1i(glGetUniformLocation(m_glProgram, "image"), 0);
	glUseProgram( 0 );
	glError();
	m_oSrc.Init(100,100);
	m_oSrc.AddRT();
}

void CFXAA::Draw()
{
	m_oSrc.SetTexture(0);
	if( m_bEnable )
	{
		glBlendFunc(GL_ONE,GL_ZERO);
		glUseProgram(m_glProgram);
		CFullScreenBlit::GetInstance()->Quad();
		glUseProgram(0);
		glError();
	} else
	{
		glBlendFunc(GL_ONE,GL_ZERO);
		CFullScreenBlit::GetInstance()->Draw();
	}
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D,0);
	glError();
}

