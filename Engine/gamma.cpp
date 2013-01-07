#include "gamma.h"
#include "opengl.h"
#include "glsl.h"
#include "fullscreenblit.h"
#include "mymacros.h"
#include "mrt.h"

#include <stdio.h>
#include <math.h>

CGamma::CGamma()
{
	m_glFragmentShader = 0;
	m_glProgram = 0;
	m_pSrc = 0;
	m_glCubeLut = 0;
}

CGamma::~CGamma()
{
	Deinit();
}

void CGamma::BeginDraw()
{
	m_pSrc->SetIt();
}

void CGamma::EndDraw()
{
	m_pSrc->Reset();
}

void CGamma::Init(bool mrt)
{
	ILog::Message("\ncompiling gamma...\n");
	unsigned int commonVert = CFullScreenBlit::GetInstance()->GetVertexShader();
	m_glFragmentShader = CGLSL::CreateFragmentShaderFromFile("gamma.frag");
	m_glProgram = CGLSL::LinkProgram(commonVert,m_glFragmentShader);

	glUseProgram( m_glProgram );
	glUniform1i(glGetUniformLocation(m_glProgram, "image"), 0);
	glUniform1i(glGetUniformLocation(m_glProgram, "lut"), 1);
	glUseProgram( 0 );
	glError();
	if(mrt)
	{
		m_pSrc = new CMRT();
		m_pSrc->Init(100,100);
		m_pSrc->AddRT();
	}
}

void CGamma::Deinit()
{
	SAFE_DELETE_GL_PROGRAM(m_glProgram);
	SAFE_DELETE_GL_SHADER(m_glFragmentShader);
	SAFE_DELETE(m_pSrc);
	SAFE_DELETE_GL_TEXTURE(m_glCubeLut);
}

void CGamma::Resize(unsigned int i_width,unsigned int i_height)
{
	if( m_pSrc )
		m_pSrc->Resize(i_width,i_height);
}

void CGamma::Draw()
{
	if( m_glCubeLut == 0)
	{
		NeutralLut();
	}
	if( m_pSrc ) 
		m_pSrc->SetTexture(0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D,m_glCubeLut);

	glBlendFunc(GL_ONE,GL_ZERO);
	glUseProgram(m_glProgram);
	CFullScreenBlit::GetInstance()->Quad();
	glUseProgram(0);
	glError();

	glBindTexture(GL_TEXTURE_3D,0);
	glActiveTexture(GL_TEXTURE0);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D,0);
	glError();
}

#include "image.h"

void CGamma::CreateRefLutPng()
{
	CImage* pImage;
	CreateNeutral(pImage);
	pImage->Save("neutral.png");
	delete pImage;
}

bool CGamma::LoadLut(const char *i_sName,bool bNeutralOnFail)
{
	bool bRet=true;
	CImage* pImage = CImage::LoadFile(i_sName);
	if( pImage == NULL )
	{
		CreateNeutral(pImage);
		bRet=false;
	}
	m_glCubeLut = pImage->GetTexture3D(4,4);
	glBindTexture(GL_TEXTURE_3D,m_glCubeLut);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_3D,0);
	return bRet;
}

void CGamma::NeutralLut()
{
	CImage* pImage;
	CreateNeutral(pImage);
	m_glCubeLut = pImage->GetTexture3D(4,4);
	glBindTexture(GL_TEXTURE_3D,m_glCubeLut);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_3D,0);
}

void CGamma::CreateNeutral(CImage*& pDest)
{
	pDest = new CImage();
	pDest->Create(16*4,16*4,3);
	unsigned char *d = pDest->GetBits();
	for(int y=0;y<16*4;y++)
	{
		for(int x=0;x<16*4;x++)
		{
			d[0] = (x % 16) * 255 / 15;
			d[1] = (y % 16) * 255 / 15;
			d[2] = ((x/16)+(y/16)*4) * 255 / 15;
			d+=3;
		}
	}
}
