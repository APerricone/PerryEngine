#pragma once

class CGLSL
{
public:
	static unsigned int CreateVertexShaderFromFile(const char* sFileName);
	static unsigned int CreateGeometryShaderFromFile(const char* sFileName);
	static unsigned int CreateFragmentShaderFromFile(const char* sFileName);

	static unsigned int CreateVertexShaderFromFiles(int nFile, ...);
	static unsigned int CreateGeometryShaderFromFiles(int nFile, ...);
	static unsigned int CreateFragmentShaderFromFiles(int nFile, ...);
	
	static unsigned int CreateVertexShader(unsigned int nSource,const char* sources[]);
	static unsigned int CreateGeometryShader(unsigned int nSource,const char* sources[]);
	static unsigned int CreateFragmentShader(unsigned int nSource,const char* sources[]);

	static unsigned int LinkProgram(unsigned int vShader,unsigned int fShader);
	static unsigned int LinkProgram(unsigned int vShader,unsigned int gShader,unsigned int fShader);

	static char* LoadShader(const char *sFileName);
private:
	// Shader utils methods
	static void CompileAndReport(unsigned int uiShader);
	static void LinkAndReport(unsigned int uiProgram);

};
