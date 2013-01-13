#include "fullscreenblit.h"
#include "opengl.h"
#include "glsl.h"

#include <stdio.h>

CFullScreenBlit CFullScreenBlit::instance;

CFullScreenBlit *CFullScreenBlit::GetInstance()
{
	return &instance;
}

CFullScreenBlit::CFullScreenBlit()
{
	m_glVertexShader	= 0; 
	m_glFragmentShader	= 0; 
	m_glProgram			= 0; 
	m_glVertexBuffer	= 0; 
}

CFullScreenBlit::~CFullScreenBlit()
{
	Deinit();
}

bool CFullScreenBlit::Init()
{
	ILog::Message("\ncompiling blit...\n");
	m_glVertexShader = CGLSL::CreateVertexShaderFromFile("blit.vert");
	m_glFragmentShader = CGLSL::CreateFragmentShaderFromFile("blit.frag");
	m_glProgram = CGLSL::LinkProgram(m_glVertexShader,m_glFragmentShader);
	glError();
	if( m_glVertexShader==0 ||
		m_glFragmentShader == 0 ||
		m_glProgram == 0 )
			return false;
	
	glGenBuffers(1, &m_glVertexBuffer);
	glError();
	if( m_glVertexBuffer==0 ) return false;
	glBindBuffer(GL_ARRAY_BUFFER_ARB, m_glVertexBuffer);
	float verticesArray[16];
	//glTexCoord2f(0,1); glVertex2f(0,0);
	//glTexCoord2f(0,0); glVertex2f(0,1);
	//glTexCoord2f(1,0); glVertex2f(1,1);
	//glTexCoord2f(1,1); glVertex2f(1,0);
	int i=0;
#define putv(px,py,tx,ty) verticesArray[i++] = px; verticesArray[i++] = py; verticesArray[i++] = tx; verticesArray[i++] = ty; 
	putv(-1, 1,0,1);
	putv(-1,-1,0,0);
	putv( 1, 1,1,1);
	putv( 1,-1,1,0);
#undef putv
	glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(float)*4*4,verticesArray, GL_STATIC_DRAW_ARB);

	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	return true;
}

void CFullScreenBlit::Deinit()
{
	SAFE_DELETE_GL_PROGRAM(m_glProgram);
	SAFE_DELETE_GL_SHADER(m_glVertexShader);
	SAFE_DELETE_GL_SHADER(m_glFragmentShader);
	SAFE_DELETE_GL_BUFFER(m_glVertexBuffer);
}

void CFullScreenBlit::Draw()
{
	glUseProgram(m_glProgram);
	Quad();
	glUseProgram(0);
}
	
void CFullScreenBlit::Quad()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_glVertexBuffer);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2,GL_FLOAT,sizeof(float)*4,0);
	glTexCoordPointer(2,GL_FLOAT,sizeof(float)*4,(void*)(sizeof(float)*2));
	
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}
