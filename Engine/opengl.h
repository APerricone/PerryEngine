#pragma once

#ifndef NO_GLEW
#	include <GL/glew.h>
#else
#	ifdef _WINDOWS
#		include <Windows.h>
#	endif //_WINDOWS
#endif //NO_GLEW
#include <GL/gl.h>

#include "log.h"

#if _DEBUG
#define glError() { \
	GLenum err = glGetError(); \
	while (err != GL_NO_ERROR) { \
		ILog::Error("%s(%i):OpenGL \"%s\"\n", __FILE__, __LINE__, (char *)gluErrorString(err)); \
		err = glGetError(); \
	} \
}
#else
#define glError() 
#endif

#define SAFE_DELETE_GL_TEXTURE(a) { if(a) glDeleteTextures(1,&a); a=0; }
#define SAFE_DELETE_GL_TEXTURES(n,a) { if(a[0]) glDeleteTextures(n,a); a[0]=0; }

#define SAFE_DELETE_GL_BUFFER(a) { if(a) glDeleteBuffers(1,&a); a=0; }
#define SAFE_DELETE_GL_BUFFERS(n,a) { if(a[0]) glDeleteBuffers(n,a); a[0]=0; }

#define SAFE_DELETE_GL_SHADER(a) { if(a) glDeleteShader(a); a=0; }
#define SAFE_DELETE_GL_PROGRAM(a) { if(a) glDeleteProgram(a); a=0; }

#define SAFE_DELETE_GL_RENDERBUFFER(a) { if(a) glDeleteRenderbuffers(1,&a); a=0; }
#define SAFE_DELETE_GL_RENDERBUFFERS(n,a) { if(a[0]) glDeleteRenderbuffers(n,a); a[0]=0; }

#define SAFE_DELETE_GL_FRAMEBUFFER(a) { if(a) glDeleteFramebuffers(1,&a); a=0; }
#define SAFE_DELETE_GL_FRAMEBUFFERS(n,a) { if(a[0]) glDeleteFramebuffers(n,a); a[0]=0; }

#ifndef GLAPIENTRY
#define GLAPIENTRY __stdcall
#endif