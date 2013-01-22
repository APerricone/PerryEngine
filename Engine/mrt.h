#pragma once

class CMRT
{
public:
	CMRT();
	~CMRT();

	void Init(unsigned int i_width,unsigned int i_height);
	void AddRTNoTex(int i_glFormat = 0);
	void AddDeptNoTexh(int i_glFormat = 0);
	void AddRT(int i_glFormat = 0,int i_glType=0,bool i_bMipMap=false);
	void AddDepth(int i_glFormat = 0);
	void AddRTTex(unsigned int i_glTexture);
	void AddDepthTex(unsigned int i_glTexture);
	void Check();

	void Resize(unsigned int i_width,unsigned int i_height);

	void SetTextures();
	static void ResetTextures();
	void SetTexture(unsigned int i_i);
	void SetTextureDepth();

	void SetIt(bool bClear = true);
	void SetSome(unsigned int i_n,...);
	void Reset();

	void UpdateMips(int i=-1);

	unsigned int GetTexture(unsigned int i_i) const { return m_glTexture[i_i]; }
	bool IsTexture(unsigned int i_i) const { return m_bTexture[i_i]; }
	unsigned int GetTextureDepth() const { return m_glDepth; }

private:
	
	unsigned int m_nTexture;
	unsigned int m_glTexture[16];
	unsigned int m_glDepth;
	unsigned int m_glFrameBuffer;
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	bool m_bDoDepth;
	// flags
	bool m_bCreated[17]; // It is created by this class
	bool m_bMipMap[17];	 // It has mip maps
	bool m_bTexture[17]; // It is a Texture

	void DeInit();

	void InternalSet(unsigned int n, const unsigned int* bufs, bool bClear = true);
};
