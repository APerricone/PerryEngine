#include "glsl.h"
#include "opengl.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

unsigned int CGLSL::CreateVertexShader(unsigned int nSource,const char* sources[])
{
	unsigned int retValue = glCreateShader(GL_VERTEX_SHADER);
	if( retValue == 0 )
	{
		ILog::Error("Creating vertex shader\n");
		exit(-1);
	}
	glShaderSource(retValue , nSource,sources, 0);
	ILog::Message("compiling vertex shader...");
	CompileAndReport(retValue);
	return retValue;
}

unsigned int CGLSL::CreateGeometryShader(unsigned int nSource,const char* sources[])
{
	unsigned int retValue = glCreateShader(GL_GEOMETRY_SHADER);
	if( retValue == 0 )
	{
		ILog::Error("Creating fragment shader\n");
		exit(-1);
	}
	glShaderSource(retValue , nSource,sources, 0);
	ILog::Message("compiling geometry shader...");
	CompileAndReport(retValue);
	return retValue;
}

unsigned int CGLSL::CreateFragmentShader(unsigned int nSource,const char* sources[])
{
	unsigned int retValue = glCreateShader(GL_FRAGMENT_SHADER);
	if( retValue == 0 )
	{
		ILog::Error("Creating fragment shader\n");
		exit(-1);
	}
	glShaderSource(retValue , nSource,sources, 0);
	ILog::Message("compiling fragment shader...");
	CompileAndReport(retValue);
	return retValue;
}

unsigned int CGLSL::LinkProgram(unsigned int vShader,unsigned int fShader)
{
	glError();

	unsigned int retValue = glCreateProgram();
	if( retValue == 0 )
	{
		ILog::Error("Creating program\n");
		exit(-1);
	}
	glAttachShader(retValue,vShader);
	glAttachShader(retValue,fShader);
	glError();

	ILog::Message("linking...");
	LinkAndReport(retValue);
	return retValue;
}

unsigned int CGLSL::LinkProgram(unsigned int vShader,unsigned int gShader,unsigned int fShader)
{
	unsigned int retValue = glCreateProgram();
	if( retValue == 0 )
	{
		ILog::Error("Creating program\n");
		exit(-1);
	}
	glAttachShader(retValue,vShader);
	glAttachShader(retValue,gShader);
	glAttachShader(retValue,fShader);
	glError();

	ILog::Message("linking...");
	LinkAndReport(retValue);
	return retValue;
}

void CGLSL::CompileAndReport(unsigned int uiShader)
{
	glCompileShader(uiShader);
	GLint ret;
	glGetShaderiv(uiShader,GL_COMPILE_STATUS, &ret);
	bool bError = ( ret != GL_TRUE );
	if(bError)
		ILog::Message("FAIL\n");
	else
		ILog::Message("OK\n");

	glGetShaderiv(uiShader, GL_INFO_LOG_LENGTH,&ret);
	if( ret <= 1 ) return;
	char *report = (char *)malloc(ret);
	
	glGetShaderInfoLog(uiShader, ret, &ret, report);
	ILog::Message(report);
	free(report);
	if(bError) 
		exit(-1);
}

void CGLSL::LinkAndReport(unsigned int uiProgram)
{
	glLinkProgram(uiProgram);
	GLint ret;
	glGetProgramiv(uiProgram,GL_LINK_STATUS, &ret);
	bool bError = ( ret != GL_TRUE );
	if(bError)
		ILog::Message("FAIL\n");
	else
		ILog::Message("OK\n");

	glGetProgramiv(uiProgram, GL_INFO_LOG_LENGTH,&ret);
	if( ret <= 1 ) return;
	char *report = (char *)malloc(ret);
	
	glGetProgramInfoLog(uiProgram, ret, &ret, report);
	ILog::Message(report);
	free(report);
	if(bError) 
		exit(-1);
}

// **************
// FILE SUPPORT
unsigned int CGLSL::CreateVertexShaderFromFile(const char* sFileName)
{
	char* sSource = LoadShader(sFileName);
	unsigned int shaderID = CreateVertexShader(1,(const char**)(&sSource));
	delete [] sSource;
	return shaderID;
}

unsigned int CGLSL::CreateGeometryShaderFromFile(const char* sFileName)
{
	char* sSource = LoadShader(sFileName);
	unsigned int shaderID = CreateGeometryShader(1,(const char**)(&sSource));
	delete [] sSource;
	return shaderID;
}

unsigned int CGLSL::CreateFragmentShaderFromFile(const char* sFileName)
{
	char* sSource = LoadShader(sFileName);
	unsigned int shaderID = CreateFragmentShader(1,(const char**)(&sSource));
	delete [] sSource;
	return shaderID;
}

unsigned int CGLSL::CreateVertexShaderFromFiles(int nFile, ...)
{
	char **asSources = new char*[nFile];
	va_list vl;
	va_start(vl,nFile);
	for(int i=0;i<nFile;i++)
	{
		const char *sFileName = va_arg(vl,char*);
		asSources[i] = LoadShader(sFileName);
	  }
	va_end(vl);

	unsigned int shaderID = CreateVertexShader(nFile,(const char**)(asSources));
	for(int i=0;i<nFile;i++)
		delete [] asSources[i];
	delete [] asSources;
	return shaderID;
}

unsigned int CGLSL::CreateGeometryShaderFromFiles(int nFile, ...)
{
	char **asSources = new char*[nFile];
	va_list vl;
	va_start(vl,nFile);
	for(int i=0;i<nFile;i++)
	{
		const char *sFileName = va_arg(vl,char*);
		asSources[i] = LoadShader(sFileName);
	  }
	va_end(vl);

	unsigned int shaderID = CreateGeometryShader(nFile,(const char**)(asSources));
	for(int i=0;i<nFile;i++)
		delete [] asSources[i];
	delete [] asSources;
	return shaderID;
}

unsigned int CGLSL::CreateFragmentShaderFromFiles(int nFile, ...)
{
	char **asSources = new char*[nFile];
	va_list vl;
	va_start(vl,nFile);
	for(int i=0;i<nFile;i++)
	{
		const char *sFileName = va_arg(vl,char*);
		asSources[i] = LoadShader(sFileName);
	  }
	va_end(vl);

	unsigned int shaderID = CreateFragmentShader(nFile,(const char**)(asSources));
	for(int i=0;i<nFile;i++)
		delete [] asSources[i];
	delete [] asSources;
	return shaderID;
}

char* CGLSL::LoadShader(const char *sFileName)
{
	FILE *f = fopen(sFileName,"rt");
	if( f == NULL )
	{
		ILog::Error("Unable to open file \"%s\"",sFileName);
		exit(-1);
	}
	fseek ( f, 0L , SEEK_END ); 
	unsigned int size = ftell(f);
	char *text = new char[size+1];
	fseek ( f, 0L , SEEK_SET ); 
	unsigned int nRead = fread(text,1,size,f);
	text [nRead] = 0;
	fclose(f);
	return text; 
}
