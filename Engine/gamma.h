#pragma once

class CMRT;
class CImage;

class CGamma
{
	CGamma(const CGamma& no_copy);
public:

	CGamma();
	~CGamma();

	bool Init(bool mrt);
	void Deinit();
	void Resize(unsigned int i_width,unsigned int i_height);
	void Draw();

	void BeginDraw();
	void EndDraw();

	bool Reinit(bool bNylon,bool bExposure,bool bFrame,bool bLut);

	// Lookup Table support
	static void CreateRefLutPng();
	static unsigned int GetRefLutTexture();
	// note: width, heght and depth of 3D Texture must be the same
	// then the image is: (w*sqrt(d))x(h*sqrt(d)) --> (a*sqrt(a))x(a*sqrt(a))
	// w=h=sqrt(a^3) --> a = (Cube root(w))^2 
	bool LoadLut(const char *i_sName,bool bNeutralOnFail = true);
	void NeutralLut();

private:
	unsigned int m_glFragmentShader;
	unsigned int m_glProgram;
	unsigned int m_glCubeLut;

	CMRT* m_pSrc;

	static void CreateNeutral(CImage*& pDest);
	//bool SetCube(CImage* pSrc);
};
