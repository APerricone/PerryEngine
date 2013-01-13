#pragma once

class CFullScreenBlit
{
	CFullScreenBlit();
	~CFullScreenBlit();
	static CFullScreenBlit instance;
public:
	static CFullScreenBlit *GetInstance();

	bool Init();
	void Draw();
	
	void Quad();

	void Deinit();

	unsigned int GetVertexShader() { return m_glVertexShader; }
private:
	unsigned int m_glVertexShader;
	unsigned int m_glFragmentShader;
	unsigned int m_glProgram;
	unsigned int m_glVertexBuffer;
};
