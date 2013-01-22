#pragma once

#include <list>

class CImage;

class IImageLoader
{
public:
	virtual const char* GetName() = 0;

	virtual bool CanLoad(const char *i_sExtension) = 0;
	virtual CImage* Load(const char *i_sPath) = 0;

	virtual bool CanSave(const char *i_sExtension) = 0;
	virtual bool Save(const char *i_sPath,const CImage* i_pImage) = 0;

protected:
	CImage* CreateInEngine();
};

//! Management of raw image
class CImage
{
public:
	CImage();
	~CImage();

	//! \param alpha se #m_iBpp==1 alpha indica se fare una tex solo alpha o solo luminosità
	void Create(unsigned int i_iDimx,unsigned int i_iDimy,unsigned int i_iBpp);
	static CImage* LoadFile(const char* i_sPath,bool bIgnoreExtension = false);
    bool Save(const char* i_sPath, IImageLoader* i_pLoader = 0);

	void Crop(unsigned int i_iX,unsigned int i_iY,unsigned int i_iDimx,unsigned int i_iDimy);
	void RemoveAlpha();

	//! create a 1D texture
	//! \note height must be 1
	//! \return -1 on error
	unsigned int GetTexture1D();
	//! create a 2D texture
	//! \note width and height must be power of 2
	//! \return -1 on error
	unsigned int GetTexture2D();
	//! create a 3D texture
	//! texture width = image width / i_iDepthAlongWidth (must be power of 2)
	//! texture height = image height / i_iDepthAlongHeight (must be power of 2)
	//! texture depth = i_iDepthAlongWidth * i_iDepthAlongHeight (must be power of 2)
	//! \return -1 on error
	unsigned int GetTexture3D(unsigned int i_iDepthAlongWidth,unsigned int i_iDepthAlongHeight);
	//! create a Texture cube
	//! It expect a horizontal cross
	//! face width = image width / 4 (must be power of 2)
	//! face height = image height / 3 (must be power of 2)
	unsigned int GetTextureCube();
	//!
	static CImage* CreateFromTexture1D(unsigned int i_glResource);
	static CImage* CreateFromTexture2D(unsigned int i_glResource);
	static CImage* CreateFromTexture3D(unsigned int i_glResource);
	static CImage* CreateFromTextureCube(unsigned int i_glResource);

	//! remove gamma correction
	void Linearize();
	//! apply gamma correction
	void Delinearize();

	const unsigned int GetDimx() const { return m_iDimx; }
	const unsigned int GetStride() const { return m_iStride; }
	const unsigned int GetDimy() const { return m_iDimy; }
	const unsigned int GetBpp() const { return m_iBpp; }
	unsigned char *GetBits() const { return m_pBits; }
	const bool GetAlphaSet() const { return m_bAlpha; }
	void SetAlphaSet(bool i_bAlpha) { m_bAlpha = i_bAlpha; }

	//! \note last registed loader is the first that will try
	static void RegisterLoader(IImageLoader* i_pLoader);
	static void UnregisterLoader(IImageLoader* i_pLoader);

private:
	//! La larghezza
	unsigned int m_iDimx;
	//! La larghezza
	unsigned int m_iStride;
	//! l'altezza
	unsigned int m_iDimy;
	//! byte per pixe puo essere 1,2,3 o 4
	unsigned int m_iBpp;
	//! i m_iBits da riempire
	unsigned char *m_pBits;
	//! 
	bool m_bAlpha;
	
	void GetFormat(unsigned int& format,unsigned int& internalFormat);
	static CImage* CreateFromOpenGLParam(int width,int height,int internalFormat,int& format);

	static std::list<IImageLoader*> s_lpLoaders;

};
