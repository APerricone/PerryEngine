#include "mrt.h"
#include "opengl.h"
#include "mymacros.h"

#include <stdlib.h>	//exit
#include <stdarg.h>	//va_

CMRT::CMRT()
{
	m_nTexture = 0;
	m_glTexture[0] = 0;
	m_glDepth = 0;
	m_glFrameBuffer =0;
}

CMRT::~CMRT()
{
	DeInit();
}

void CMRT::DeInit()
{
	SAFE_DELETE_GL_FRAMEBUFFER(m_glFrameBuffer);
	if( m_bCreated[16])
	{
		if( m_bTexture[16] )
		{
			SAFE_DELETE_GL_TEXTURE(m_glDepth);
		} else
		{
			SAFE_DELETE_GL_RENDERBUFFER(m_glDepth);
		}
	} 
	m_glDepth = 0;
	if(m_nTexture)
	{
		for(unsigned int i=0;i<m_nTexture;i++)
		{
			if( m_bCreated[i] )
			{
				if( m_bTexture[i] )
				{
					SAFE_DELETE_GL_TEXTURE(m_glTexture[i]);
				} else
				{
					SAFE_DELETE_GL_RENDERBUFFER(m_glTexture[i]);
				}
			}
		}
	}
	m_nTexture = 0;
}

void CMRT::Init(unsigned int i_width,unsigned int i_height)
{
	m_nTexture = 0;
	// create a framebuffer object, you need to delete them when program exits.
	glGenFramebuffers(1, &m_glFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glError();

	m_iWidth = i_width;
	m_iHeight = i_height;
	m_bDoDepth = false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glError();
}

void CMRT::AddRTNoTex(int i_glFormat)
{
	if( i_glFormat == 0) i_glFormat = GL_RGBA8;
	m_bCreated[m_nTexture] = true;
	m_bTexture[m_nTexture] = false;
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glError();

	glGenRenderbuffers(1,&m_glTexture[m_nTexture]);
	glBindRenderbuffer(GL_RENDERBUFFER, m_glTexture[m_nTexture]);
	glRenderbufferStorage(GL_RENDERBUFFER, i_glFormat, m_iWidth, m_iHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+m_nTexture, GL_RENDERBUFFER, m_glTexture[m_nTexture]);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glError();

	m_nTexture++;
}

void CMRT::AddDeptNoTexh(int i_glFormat)
{
	if( i_glFormat == 0) i_glFormat = GL_DEPTH_COMPONENT16;
	m_bCreated[16] = true;
	m_bTexture[16] = false;
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glError();

	glGenRenderbuffers(1,&m_glDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_glDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, i_glFormat, m_iWidth, m_iHeight);
	m_nTexture++;

	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_glDepth);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glError();
}

void CMRT::AddRT(int i_glFormat,int i_glType,bool i_bMipMap)
{
	if( i_glFormat == 0) i_glFormat = GL_RGBA8;
	if( i_glType == 0) i_glType = GL_UNSIGNED_BYTE;
	m_bCreated[m_nTexture] = true;
	m_bTexture[m_nTexture] = true;
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glError();

	glGenTextures(1,&m_glTexture[m_nTexture]);
	glBindTexture(GL_TEXTURE_2D,m_glTexture[m_nTexture]);
	if( i_bMipMap )
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D,0,i_glFormat,m_iWidth,m_iHeight,0,GL_RGBA,i_glType,NULL);
	m_bMipMap[m_nTexture] = i_bMipMap;
	if( i_bMipMap )
	{
		int l = 1, w = m_iWidth,h=m_iHeight;
		do
		{
			w >>= 1;
			h >>= 1;
			glTexImage2D(GL_TEXTURE_2D,l,i_glFormat,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
		} while( w!=1 && h!=1);
	}


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+m_nTexture, GL_TEXTURE_2D, m_glTexture[m_nTexture], 0);
	glError();

	glBindTexture(GL_TEXTURE_2D, 0);
	m_nTexture++;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glError();
}

void CMRT::AddDepth(int i_glFormat)
{
	if( i_glFormat == 0) i_glFormat = GL_DEPTH_COMPONENT16;

	m_bCreated[16] = true;
	m_bTexture[16] = true;
	m_bDoDepth = true;
	glGenTextures(1,&m_glDepth);
	glBindTexture(GL_TEXTURE_2D,m_glDepth);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	GLfloat color[4]={10,10,10,10};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	glTexImage2D( GL_TEXTURE_2D, 0, i_glFormat, m_iWidth, m_iHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_glDepth, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glError();
}

void CMRT::AddRTTex(unsigned int i_glTexture)
{
	m_bCreated[m_nTexture] = false;
	m_bTexture[m_nTexture] = true;
	m_glTexture[m_nTexture] = i_glTexture;

	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+m_nTexture, GL_TEXTURE_2D, m_glTexture[m_nTexture], 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glError();

	m_nTexture++;
}

void CMRT::AddDepthTex(unsigned int i_glTexture)
{
	m_bDoDepth = true;
	m_bCreated[16] = false;
	m_bTexture[16] = true;
	m_glDepth = i_glTexture;

	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_glDepth, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glError();
}

void CMRT::Check()
{
	ILog::Message("Checking render targer...");
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE )
	{
		ILog::Message("FAIL(%x)\n", status );
		ILog::Error("FAIL");
		exit(-1);
	}
	ILog::Message("OK\n");
}

void CMRT::Resize(unsigned int i_width,unsigned int i_height)
{
	if( i_width == m_iWidth && i_height == m_iHeight ) return;
	m_iWidth = i_width;
	m_iHeight = i_height;
	for(unsigned int i=0;i<m_nTexture;i++)
		if(m_bCreated[i])
		{
			glBindTexture(GL_TEXTURE_2D,m_glTexture[i]);
			GLint f;
			glGetTexLevelParameteriv(GL_TEXTURE_2D,0, GL_TEXTURE_INTERNAL_FORMAT, &f);
			glTexImage2D(GL_TEXTURE_2D,0,f,m_iWidth,m_iHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
			if( m_bMipMap[i] )
			{
				int l = 1, w = m_iWidth,h=m_iHeight;
				do
				{
					w >>= 1;
					h >>= 1;
					glTexImage2D(GL_TEXTURE_2D,l,f,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
				} while( w!=1 && h!=1);
			}
		}
	if(m_bDoDepth && m_bCreated[16])
	{
		glBindTexture(GL_TEXTURE_2D,m_glDepth);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_iWidth, m_iHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	}
	glBindTexture(GL_TEXTURE_2D,0);
}

void CMRT::SetTextures()
{
	for(unsigned int i=0;i<m_nTexture;i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_glTexture[i]);
	}
	glActiveTexture(GL_TEXTURE0 );
}


void CMRT::ResetTextures()
{
	for(unsigned int i=0;i<16;i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0 );
}

void CMRT::SetTexture(unsigned int i)
{
	if(i<m_nTexture)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,m_glTexture[i]);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,0);
	}
	glError();
}

void CMRT::SetTextureDepth()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_glDepth);
}

void CMRT::SetSome(unsigned int i_n,...)
{
	GLenum buffers[16];
	va_list vl;
	va_start(vl,i_n);
	for(unsigned int i=0;i<i_n;i++)
		buffers[i] = GL_COLOR_ATTACHMENT0 +  va_arg(vl,unsigned int);
	va_end(vl);
	InternalSet(i_n, buffers);
}

void CMRT::SetIt()
{
	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5,GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7,GL_COLOR_ATTACHMENT8,GL_COLOR_ATTACHMENT9,
		GL_COLOR_ATTACHMENT10,GL_COLOR_ATTACHMENT11,GL_COLOR_ATTACHMENT12,
		GL_COLOR_ATTACHMENT13,GL_COLOR_ATTACHMENT14,GL_COLOR_ATTACHMENT15 };
	InternalSet(m_nTexture, buffers);
}

void CMRT::InternalSet(unsigned int n, const unsigned int* bufs)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
	glViewport(0, 0, m_iWidth, m_iHeight);
	glDrawBuffers(n,bufs);

	if( m_bDoDepth )
	{
		if( m_bCreated[16] )
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		else
			glClear( GL_COLOR_BUFFER_BIT );
	} else
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glClear( GL_COLOR_BUFFER_BIT);
	}
}

void CMRT::Reset()
{
	if( !m_bDoDepth )
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}

	glError();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	//int width,height;
	//glfwGetWindowSize(&width,&height);
	//glViewport(0, 0, width,height);
	
	glError();
}

void CMRT::UpdateMips(int i)
{
	int nMin = i, nMax = i+1;
	if( i==-1) 
	{
		nMin = 0; 
		nMax = m_nTexture;
	}
	glEnable(GL_TEXTURE_2D);
	for(int i=nMin;i<nMax;i++)
		if( m_bMipMap[i])
		{
			glBindTexture(GL_TEXTURE_2D, m_glTexture[i] ); 
			glGenerateMipmap(GL_TEXTURE_2D);

		}
	glDisable(GL_TEXTURE_2D);
};
