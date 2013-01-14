#include "gamma.h"
#include "opengl.h"
#include "glsl.h"
#include "fullscreenblit.h"
#include "mymacros.h"
#include "mrt.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

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

bool CGamma::Init(bool mrt)
{
	if(!Reinit(true,true,true,true))
		return false;

	if(mrt)
	{
		m_pSrc = new CMRT();
		m_pSrc->Init(100,100);
		m_pSrc->AddRT();
	}
	return true;
}

bool CGamma::Reinit(bool bNylon,bool bExposure,bool bFrame,bool bLut)
{
	ILog::Message("\ncompiling gamma...\n");
	SAFE_DELETE_GL_PROGRAM(m_glProgram);
	SAFE_DELETE_GL_SHADER(m_glFragmentShader);
	bool missFile(false);
	char **asSources = new char*[2];
	asSources[0] = CGLSL::LoadShader("gamma.frag");
	if( asSources[0]==0 )
		return false;
	asSources[1] = new char[1000];
	strcpy(asSources[1],"void main()\n{\n\t// base color\n");
	if( bNylon )
	{
		strcat(asSources[1],"\tvec3 color = nylon();\n");
	} else
	{
		strcat(asSources[1],"\tvec3 color = texture(image, TexCoord.xy).xyz;\n");
	}
	if(bExposure) strcat(asSources[1],"\tcolor = exposure(color);\n");
	if(bFrame) strcat(asSources[1],"\tcolor = frame(color);\n");
	strcat(asSources[1],"\tcolor = clamp(pow( color,vec3(1/2.2)),0,1-1/32.0);\n");
	if(bLut) strcat(asSources[1],"\tcolor = texture(lut,color,0).xyz;\n");
	strcat(asSources[1],"\tgl_FragData.xyz = color;\ngl_FragData.w = getLum( gl_FragData.xyz );\n}\n");
	ILog::Message("fragment gamma variable part len: %i\n",strlen(asSources[1]));
	unsigned int commonVert = CFullScreenBlit::GetInstance()->GetVertexShader();
	m_glFragmentShader = CGLSL::CreateFragmentShader(2,(const char**)(asSources));
	m_glProgram = CGLSL::LinkProgram(commonVert,m_glFragmentShader);

	delete [] asSources[1];
	delete [] asSources[0];
	delete [] asSources;

	if(m_glFragmentShader == 0 || m_glProgram == 0) return false;

	glUseProgram( m_glProgram );
	glUniform1i(glGetUniformLocation(m_glProgram, "image"), 0);
	glUniform1i(glGetUniformLocation(m_glProgram, "lut"), 1);
	glUseProgram( 0 );
	glError();
	return true;
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
