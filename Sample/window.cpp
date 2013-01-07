#include "window.h"

#include "log.h"

#include "mesh.h"
#include "dynamicmesh.h"

#include "opengl.h"
#include "glsl.h"

#include "mymacros.h"

CWindow::CWindow(const float3& f3Dir)
{
	m_fInt = 4;
	CDynamicMesh oCreator;
	float3 f3Center = f3Dir * 50.f;
	const float s = 0.1f;
	const float d = 10;
	const float r = 0.2f;
	oCreator.AddBox(f3Center+float3( r, r,s),f3Center+float3( d, d,s));
	oCreator.AddBox(f3Center+float3(-d, r,s),f3Center+float3(-r, d,s));
	oCreator.AddBox(f3Center+float3( r,-d,s),f3Center+float3( d,-r,s));
	oCreator.AddBox(f3Center+float3(-d,-d,s),f3Center+float3(-r,-r,s));
	m_pMesh = new CMesh();
	oCreator.CreateMesh(*m_pMesh);

	ILog::Message("\ncompiling simplewriter...\n");
	m_glVertex = CGLSL::CreateVertexShaderFromFile("simplewriter.vert");
	m_glFragment = CGLSL::CreateFragmentShaderFromFile("simplewriter.frag");
	m_glProgram = CGLSL::LinkProgram(m_glVertex,m_glFragment);
	glError();
}

CWindow::~CWindow()
{
	SAFE_DELETE(m_pMesh);
	SAFE_DELETE_GL_PROGRAM( m_glProgram );
	SAFE_DELETE_GL_SHADER( m_glVertex );
	SAFE_DELETE_GL_SHADER( m_glFragment );
}

void CWindow::Draw()
{
	glUseProgram( m_glProgram );
	glError();

	glColor4f(m_fInt,m_fInt,m_fInt,1);
	m_pMesh->Draw();

	glUseProgram( 0 );
	glError();
}
